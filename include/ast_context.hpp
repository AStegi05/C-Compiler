#pragma once
#include "ast_type_specifier.hpp"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <unordered_set>

namespace ast {

struct Variable {
    std::string name;
    int stackOffset;
    bool isParameter;
    TypeSpecifier type;
    int arraySize;
    int pointerDepth;

    int getStackOffset() const {
        return stackOffset;
    }

    bool isArray() const {
        return arraySize > 0;
    }

    bool isFloat() const {
        return type == TypeSpecifier::FLOAT;
    }

    bool isDouble() const {
        return type == TypeSpecifier::DOUBLE;
    }

    bool isPointer() const {
        return pointerDepth > 0;
    }

    bool isChar() const {
        return type == TypeSpecifier::CHAR;
    }

    bool isUnsigned() const {
        return type == TypeSpecifier::UNSIGNED;
    }

    Variable() : name(""), stackOffset(0), isParameter(false), type(TypeSpecifier::INT), arraySize(0), pointerDepth(0) {}

    Variable(const std::string& n, int offset, bool isParam, TypeSpecifier t, int size = 0, int pointer = 0)
        : name(n), stackOffset(offset), isParameter(isParam), type(t), arraySize(size), pointerDepth(pointer) {}
};



class Context {
    private:

        // Stack of scopes, with the innermost scope at the back
        std::vector<std::unordered_map<std::string, Variable>> scopes;

        std::unordered_map<std::string, Variable> globalVariables;

        // String table
        std::unordered_map<std::string, int> string_table;
        int string_counter = 0;

        // Track the current stack offset for allocating variables
        int currentStackOffset = 0;

        int parameterOffset = 16;
        // Track which parameter we're processing
        int currentIntParamIndex = 0;
        int currentFloatParamIndex = 0;

        std::string currentFunctionName;

        struct LoopLabelContext {
            std::string endLabel;   // Label for break
            std::string startLabel; // Label for continue
            LoopLabelContext* next;
            LoopLabelContext(const std::string& end, const std::string& start, LoopLabelContext* nxt = nullptr)
                : endLabel(end), startLabel(start), next(nxt) {}
        };

        // Head of the linked list (nullptr if no loop context is active)
        LoopLabelContext* loopLabelContextHead = nullptr;


    public:
        std::unordered_set<std::string> declaredFunctions = {};

        Context() {
            // Create initial scope
            enterScope();
        }

        // Create a new scope
        void enterScope() {
            // Add a new empty scope to the back of the vector
            scopes.push_back(std::unordered_map<std::string, Variable>());
            //std::cerr << "DEBUG: Entered scope #" << scopes.size() << std::endl;
        }

        // Exit the current scope
        void exitScope() {
            if (scopes.size() <= 1) {
                throw std::runtime_error("Cannot exit global scope");
            }
            scopes.pop_back();
            //std::cerr << "DEBUG: Exited to scope #" << scopes.size() << std::endl;
        }

        struct ParamInfo {
            int stackOffset;
            int regIntIndex;
            int regFloatIndex;
            bool isFloat;
        };


        // Parameters: Passed to functions, initialized by the caller, tracked with positive stack offsets, and marked as parameters
        int declareParameter(const std::string& name, TypeSpecifier type, int pointerDepth = 0) {
            if (scopes.back().find(name) != scopes.back().end()) {
                throw std::runtime_error("Parameter '" + name + "' already declared in current scope");
            }

            int offset = parameterOffset;

            Variable var{name, offset, true, type, 0, pointerDepth};
            scopes.back()[name] = var;


            parameterOffset += 8;  // Move to next parameter position // Keep 8/16-byte alignment for RISC-V
            if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE) {
                currentFloatParamIndex++;
            } else {
                currentIntParamIndex++;
            }
            return offset;
        }



        // Variables: Defined within function bodies, explicitly initialized, tracked with negative stack offsets, and marked as non-parameters
        int declareVariable(const std::string& name, TypeSpecifier type, int pointerDepth = 0) {
            // Check if variable already exists in the current scope only
            if (scopes.back().find(name) != scopes.back().end()) {
                throw std::runtime_error("Variable '" + name + "' already declared in current scope");
            }

            // We use negative offsets to grow the stack downwards
            if (type == TypeSpecifier::DOUBLE && pointerDepth == 0) {
                currentStackOffset -= 8;
            } else {
                currentStackOffset -= 4;
            }

            Variable var{name, currentStackOffset, false, type, 0, pointerDepth};
            scopes.back()[name] = var;

            // std::cerr << "DEBUG: Declared variable " << name << " with offset " << currentStackOffset
            // << " in scope " << scopes.size() << ", type: " << type << ", pointer level: " << pointerDepth << std::endl;

            return currentStackOffset;
        }


        int declareGlobalVariable(const std::string& name, TypeSpecifier type, int pointerDepth = 0) {
            if (globalVariables.find(name) != globalVariables.end()) {
                throw std::runtime_error("Global variable '" + name + "' already declared");
            }


            static int globalOffset = 0;
            int offset = globalOffset;
            globalOffset += 4;

            // Create and store the global variable info with special marker for globals
            Variable var{name, offset, false, type, 0, pointerDepth};
            globalVariables[name] = var;

            // std::cerr << "DEBUG: Declared global variable " << name << " with offset " << offset
            //   << ", type: " << type << ", pointer level: " << pointerDepth << std::endl;

            return offset;
        }

        int declareArray(const std::string& name,int size) {

            if (scopes.back().find(name) != scopes.back().end()) {
                throw std::runtime_error("Array '" + name + "' already declared in current scope");
            }

            // We use negative offsets to grow the stack downwards
            currentStackOffset -= (size * 4);

            // Create and store the variable info
            Variable var{name, currentStackOffset, false, TypeSpecifier::INT, size};
            scopes.back()[name] = var;

            // std::cerr << "DEBUG: Declared array " << name << " with size " << size
            //     << " and offset " << currentStackOffset
            //     << " in scope " << scopes.size() << std::endl;

            return currentStackOffset;
        }

        // Define a string literal and return its index
        int define_string(const std::string& str) {
            auto it = string_table.find(str);

            if (it != string_table.end()) {
                return it->second;
            }

            int index = string_counter++;
            string_table[str] = index;
            return index;
        }

        // Get all strings for data section generation
        const std::unordered_map<std::string, int>& get_string_table() const {
            return string_table;
        }


        // Look up a variable
        // return pointer to object if found or nullptr if not found
        // Find a variable in all scopes, starting from innermost
        // scopes.rbegin() returns a reverse iterator pointing to the last element in the scopes vector (the innermost scope).
        // scopes.rend() returns a reverse iterator pointing to the theoretical element preceding the first element in the scopes vector (the outermost scope).
        Variable* findVariable(const std::string& name) {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {

                auto var_it = it->find(name);

                if (var_it != it->end()) {
                    // std::cerr << "DEBUG: Found variable " << name << " in a local scope" << std::endl;
                    return &(var_it->second);
                }
            }

            // If not found in local scopes, check global variables
            auto global_it = globalVariables.find(name);

            if (global_it != globalVariables.end()) {
                // std::cerr << "DEBUG: Found variable " << name << " in global scope" << std::endl;
                return &(global_it->second);
            }

            return nullptr;
        }

        bool isGlobalScope() const {
            return scopes.size() == 1 && currentFunctionName.empty();
        }

        bool isGlobalVariable(const std::string& name) const {
            return globalVariables.find(name) != globalVariables.end();
        }


         // Get total stack size needed (including alignment)
         int getTotalStackSize() const {
            int localSize = -currentStackOffset;
            //int paramSize = parameterOffset - 16;  // Sub initial offset
            int size = localSize + 16;  // 16 bytes for saved fp and ra
            // Round up to 16-byte alignment for RISC-V
            return (size + 15) & ~15;
        }

        // New method that returns both offset and register index
        ParamInfo getParamInfo(const std::string& name) {
            auto* var = findVariable(name);
            if (!var || !var->isParameter) {
                throw std::runtime_error("Variable '" + name + "' is not a parameter");
            }
            return ParamInfo{var->stackOffset, currentIntParamIndex - 1, currentFloatParamIndex - 1 ,var->type == TypeSpecifier::FLOAT};  // -1 because we incremented in declareParameter
        }

        void resetContext() {
            std::cerr << "DEBUG: Resetting context" << std::endl;
            currentStackOffset = 0;
            parameterOffset = 16;
            currentIntParamIndex = 0;
            currentFloatParamIndex = 0;
            scopes.clear();
            enterScope(); // Create initial scope for function
            // std::cerr << "DEBUG: Created initial function scope" << std::endl;
        }

        std::string CreateLabel(const std::string& prefix) {
            static int label_counter = 0;
            return prefix + "_" + std::to_string(label_counter++);
        }
        std::string CreateEndStartLabel(const std::string& prefix) {
            static int label_counter = 0;
            return prefix + "_" + std::to_string(label_counter++);
        }

        void pushLoopLabelContext(const std::string end_label,const std::string start_label ) {
            loopLabelContextHead = new LoopLabelContext(end_label, start_label, loopLabelContextHead);
        }

        // Pop the current loop label context from the linked list
        void popLoopLabelContext() {
            if (loopLabelContextHead) {
                LoopLabelContext* temp = loopLabelContextHead;
                loopLabelContextHead = loopLabelContextHead->next;
                delete temp;
            } else {
                std::cerr << "DEBUG: Loop label context stack is empty!" << std::endl;
            }
        }

        // Get the current (top) loop label context. Returns nullptr if no loop context is active
        const LoopLabelContext* topLoopLabelContext() const {
            return loopLabelContextHead;
        }

        // NEW: Function name tracking
        void setCurrentFunctionName(const std::string& name) {
            currentFunctionName = name;
        }

        std::string getCurrentFunctionName() const {
            return currentFunctionName;
        }
        bool isFunction(std::string name) const{
            return declaredFunctions.find(name) != declaredFunctions.end();
        }
 };
} // namespace ast

