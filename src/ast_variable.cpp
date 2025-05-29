#include "ast_variable.hpp"
#include "ast_context.hpp"
#include "ast_identifier.hpp"
#include "ast_array.hpp"
#include "ast_string.hpp"
#include <string>
#include <iostream>

//to gówno jest tak długi nie chce mi się tego czytać a ja to napisałem
namespace ast {

    void VariableDeclare::EmitRISC(std::ostream& stream, Context& context) const {

        std::string name = init_declarator_->getIdentifier();
        int pointerDepth = init_declarator_->isPointer();

        // remember globals go first dumdum
        if (context.isGlobalScope()) {
            if (context.findVariable(name) != nullptr) {
                throw std::runtime_error("Variable '" + name + "' already declared");
            }

            context.declareGlobalVariable(name, declaration_specifier_);

            stream << ".data" << std::endl;
            stream << name << ":" << std::endl;

            if (init_declarator_->hasInitializer()) {
            if (pointerDepth > 0) {
                if (init_declarator_->hasConstantValue()) {

                    int value = init_declarator_->getConstantValue();
                    stream << ".word " << value << std::endl;

                } else {
                    stream << ".word 0" << std::endl;
                }
            } else if (declaration_specifier_ == TypeSpecifier::CHAR) {
                if (init_declarator_->hasConstantValue()) {

                    int value = init_declarator_->getConstantValue();
                    stream << ".byte " << value << std::endl;
                    stream << ".zero 3  " << std::endl;

                } else {
                    stream << ".byte 0" << std::endl;
                    stream << ".zero 3" << std::endl;
                }
            } else if (declaration_specifier_ == TypeSpecifier::FLOAT) {
                if (init_declarator_->hasConstantFloatValue()) {

                    float value = init_declarator_->getConstantFloatValue();
                    // convert float to int
                    union {
                        float f;
                        uint32_t i;
                    } convert;

                    convert.f = value;
                    stream << ".word " << convert.i << std::endl;

                } else {
                    stream << ".word 0" << std::endl;
                }
            } else if (declaration_specifier_ == TypeSpecifier::DOUBLE) {
                    if (init_declarator_->hasConstantDoubleValue()) {

                        double value = init_declarator_->getConstantDoubleValue();
                        // convert double to two ints
                        union {
                            double d;
                            uint64_t i;
                        } convert;

                        convert.d = value;
                        uint32_t low = convert.i & 0xFFFFFFFF;
                        uint32_t high = (convert.i >> 32) & 0xFFFFFFFF;
                        stream << ".word " << low << std::endl;
                        stream << ".word " << high << std::endl;

                    } else {
                        stream << ".word 0" << std::endl;
                        stream << ".word 0" << std::endl;
                    }
                } else {
                    if (init_declarator_->hasConstantValue()) {

                        int value = init_declarator_->getConstantValue();
                        stream << ".word " << value << std::endl;

                    } else {
                        stream << ".word 0  # default int value" << std::endl;
                    }
                }
                } else { // i realise so of these specifier might be redundant but better safe than sorry
                    if (pointerDepth > 0) {
                        stream << ".word 0" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::CHAR) {
                        stream << ".byte 0" << std::endl;
                        stream << ".align 2" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::FLOAT) {
                        stream << ".word 0" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::DOUBLE) {
                        stream << ".word 0" << std::endl;
                        stream << ".word 0" << std::endl;
                    } else {
                        stream << ".word 0" << std::endl;
                    }
                }

            stream << ".text" << std::endl;
        } else {
            if (init_declarator_->isArray()) {

                int size = init_declarator_->isArray();

                if (context.findVariable(name) != nullptr) {
                    throw std::runtime_error("Array variable '" + name + "' already declared");
                }

                int offset = context.declareArray(name, size);

                if (declaration_specifier_ == TypeSpecifier::CHAR) {

                    const Node* initializer = init_declarator_->getInitializer();

                    if (initializer) {

                        const StringLiteral* strL = static_cast<const StringLiteral*>(initializer);

                        if (strL) {

                            const std::string& str = strL->getValue();

                            if (size < static_cast<int>(str.length() + 1)) {
                                throw std::runtime_error("String initializer too long for array");
                            }

                            initializer->EmitRISC(stream, context);

                            stream << "mv a1, a5" << std::endl;
                            stream << "addi a0, s0, " << offset << std::endl;

                            std::string loop_label = context.CreateLabel("string_copy_loop");
                            std::string end_label = context.CreateLabel("string_copy_end");

                            stream << loop_label << ":" << std::endl;
                            stream << "lb t0, 0(a1)" << std::endl;
                            stream << "sb t0, 0(a0)" << std::endl;
                            stream << "beqz t0, " << end_label << std::endl;
                            stream << "addi a0, a0, 1" << std::endl;
                            stream << "addi a1, a1, 1" << std::endl;
                            stream << "j " << loop_label << std::endl;
                            stream << end_label << ":" << std::endl;

                            return;
                        }
                    }
                }


                const Node* initializer = init_declarator_->getInitializer();
                if (initializer) {

                    const auto values = init_declarator_->getConstList();

                    if (values.size() == static_cast<size_t>(size)) {

                        for (size_t i = 0; i < values.size(); ++i) {
                            stream << "li a5, " << std::get<int>(values[i]) << std::endl;
                            stream << "sw a5, " << offset + i * 4 << "(s0)" << std::endl;
                        }

                        if (values.size() < static_cast<size_t>(size)) {
                            stream << "li a5, 0" << std::endl;

                            for (size_t i = values.size(); i < static_cast<size_t>(size); ++i) {
                                stream << "sw a5, " << offset + i * 4 << "(s0)" << std::endl;
                            }
                        }
                    } else {
                        throw std::runtime_error("Invalid initializer for array '" + name + "'");
                    }
                } else {
                    stream << "li a5, 0" << std::endl;
                    for (int i = 0; i < size; ++i) {
                        stream << "sw a5, " << offset + i * 4 << "(s0)" << std::endl;
                    }
                }
            } else {
                int offset = context.declareVariable(name, declaration_specifier_, pointerDepth);

                if (declaration_specifier_ == TypeSpecifier::CHAR && pointerDepth > 0) {

                    const Node* initializer = init_declarator_->getInitializer();

                    if (initializer) {

                        const StringLiteral* strL = dynamic_cast<const StringLiteral*>(initializer);

                        if (strL) {
                            initializer->EmitRISC(stream, context);

                            stream << "sw a5, " << offset << "(s0)" << std::endl;

                            return;
                        }
                    }
                }

                if (init_declarator_) {

                    init_declarator_->EmitRISC(stream, context);

                    if (pointerDepth > 0) {
                        stream << "sw a5, " << offset << "(s0)" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::CHAR) {
                        stream << "sb a5, " << offset << "(s0)" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::FLOAT) {
                        stream << "fsw fa0, " << offset << "(s0)" << std::endl;
                    } else if (declaration_specifier_ == TypeSpecifier::DOUBLE) {
                        stream << "fsd fa0, " << offset << "(s0)" << std::endl;
                    } else {
                        stream << "sw a5, " << offset << "(s0)" << std::endl;
                    }
                }
            }
        }
    }



    void VariableDeclare::Print(std::ostream& stream) const
    {
        stream << declaration_specifier_ << " ";
        init_declarator_->Print(stream);
        stream << ";" << std::endl;
    }


    void VariableAssign::EmitRISC(std::ostream& stream, Context& context) const {

        auto var = context.findVariable(identifier_);

        if (!var) {
            throw std::runtime_error("Variable not found: " + identifier_);
        }

        if (var->type == TypeSpecifier::CHAR && (var->isArray() || var->isPointer())) {

            const StringLiteral* strL = dynamic_cast<const StringLiteral*>(expression_.get());

            if (strL) {
                expression_->EmitRISC(stream, context);

                stream << "mv a1, a5" << std::endl;

                if (context.isGlobalVariable(identifier_)) {
                    stream << "la a0, " << identifier_ << std::endl;
                } else if (var->isArray()) {
                    stream << "addi a0, s0, " << var->getStackOffset() << std::endl;
                } else {
                    stream << "lw a0, " << var->getStackOffset() << "(s0)" << std::endl;
                }

                std::string loop_label = context.CreateLabel("string_copy_loop");
                std::string end_label = context.CreateLabel("string_copy_end");

                stream << loop_label << ":" << std::endl;
                stream << "lb t0, 0(a1)" << std::endl;
                stream << "sb t0, 0(a0)" << std::endl;
                stream << "beqz t0, " << end_label << std::endl;
                stream << "addi a0, a0, 1" << std::endl;
                stream << "addi a1, a1, 1" << std::endl;
                stream << "j " << loop_label << std::endl;
                stream << end_label << ":" << std::endl;

                if (context.isGlobalVariable(identifier_)) {
                    stream << "la a5, " << identifier_ << std::endl;
                } else if (var->isArray()) {
                    stream << "addi a5, s0, " << var->getStackOffset() << std::endl;
                } else {
                    stream << "lw a5, " << var->getStackOffset() << "(s0)" << std::endl;
                }

                return;
            }
        }

        expression_->EmitRISC(stream, context);

        if (context.isGlobalVariable(identifier_)) {

            stream << "la t0, " << identifier_ << std::endl;

            if (var->isChar() && !var->isPointer()) { // Only use byte store for char types that aren't pointers
                stream << "sb a5, 0(t0)" << std::endl;
            } else if (var->isFloat()) {
                stream << "fsw fa0, 0(t0)" << std::endl;
            } else if (var->isDouble()) {
                stream << "fsd fa0, 0(t0)" << std::endl;
            } else {
                stream << "sw a5, 0(t0)" << std::endl;
            }

        } else {

            if (var->isArray()) {
                throw std::runtime_error("Cannot assign to array '" + identifier_ + "' directly");
            } else {

                if (var->isChar() && !var->isPointer()) {
                    stream << "sb a5, " << var->getStackOffset() << "(s0)" << std::endl;
                } else if (var->isFloat()) {
                    stream << "fsw fa0, " << var->getStackOffset() << "(s0)" << std::endl;
                } else if (var->isDouble()) {
                    stream << "fsd fa0, " << var->getStackOffset() << "(s0)" << std::endl;
                } else {
                    stream << "sw a5, " << var->getStackOffset() << "(s0)" << std::endl;
                }
            }

        }
    }

    void VariableAssign::Print(std::ostream& stream) const
    {
        stream << identifier_;
        stream << " = ";
        expression_->Print(stream);
        stream << ";" << std::endl;
    }


    void VariableCall::EmitRISC(std::ostream& stream, Context& context) const {

        std::string name = init_declarator_->getIdentifier();
        auto var = context.findVariable(name);

        if (context.isGlobalVariable(name)) {

            stream << "la t0, " << name << std::endl;

            if (var->isPointer()) {
                stream << "lw a5, 0(t0)" << std::endl;
            } else if (var->isChar()) {
                stream << "lbu a5, 0(t0)" << std::endl;
            } else if (var->isFloat()) {
                stream << "flw fa0, 0(t0)" << std::endl;
            } else if (var->isDouble()) {
                stream << "fld fa0, 0(t0)" << std::endl;
            } else {
                stream << "lw a5, 0(t0)" << std::endl;
            }

        } else {
            if (var->isArray()) {

                stream << "addi a5, s0, " << var->getStackOffset() << std::endl;

            } else if (var->isPointer()) {

                stream << "lw a5, " << var->getStackOffset() << "(s0)" << std::endl;

            } else {
                if (var->isChar()) {
                    stream << "lbu a5, " << var->getStackOffset() << "(s0)" << std::endl;
                } else if (var->isFloat()) {
                    stream << "flw fa0, " << var->getStackOffset() << "(s0)" << std::endl;
                } else if (var->isDouble()) {
                    stream << "fld fa0, " << var->getStackOffset() << "(s0)" << std::endl;
                } else {
                    stream << "lw a5, " << var->getStackOffset() << "(s0)" << std::endl;
                }
            }
        }
    }

    bool VariableCall::EmitValue(std::ostream& stream, Context& context) const {

        std::string name = init_declarator_->getIdentifier();
        auto var = context.findVariable(name);

        if (var->isArray()) {
            return false;
        }
        if (var->isParameter) {
            stream << "    addi t0, s0, " << var->getStackOffset() << std::endl;
        } else {
            stream << "    addi t0, s0, " << var->getStackOffset() << std::endl;
        }

        return true;
    }

    void VariableCall::Print(std::ostream& stream) const
    {
        init_declarator_->Print(stream);
    }


} // namespace ast
