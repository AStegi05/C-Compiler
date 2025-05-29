#include "ast_array.hpp"
#include "ast_context.hpp"
#include <iostream>
#include <stdexcept>

namespace ast {

void DeclareArray::EmitRISC(std::ostream&, Context&) const {
    // look in Variable Declaration
}

void DeclareArray::Print(std::ostream& stream) const {
    identifier_->Print(stream);
    stream << "[" << size_ << "]";
}



// This method retrieves a list of constant values from the array initializer.
// It iterates over the elements of the array and collects their constant values into a vector
std::vector<std::variant<int, float, double>> InitializeArray::getConstList() const {
    std::vector<std::variant<int, float, double>> values;
    const NodeList* list = getElements();

    for (const auto& element : list->getNodes()) { values.push_back(element->getConst()); }

    return values;
}


void InitializeArray::EmitRISC(std::ostream& stream, Context& context) const {

    const NodeList* list = getElements();
    list->EmitRISC(stream, context);
}

void InitializeArray::Print(std::ostream& stream) const {
    stream << "{ ";
    const NodeList* list = getElements();

    const auto& nodes = list->getNodes();
    for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i]->Print(stream);
            if (i < nodes.size() - 1) {
                stream << ", ";
            }
    }

    stream << " }";
}


// The goal is to load the value from the array element into a5.
// The index value is saved to t0 to keep a5 free for loading the final value.
// The base address is loaded into t1, and the final address is calculated in t0.
void AccessArray::EmitRISC(std::ostream& stream, Context& context) const {
    std::string arrayName = array_->getIdentifier();

    auto* var = context.findVariable(arrayName);
    if (!var) {
        throw std::runtime_error("Array/pointer not found: " + arrayName);
    }

    bool isActualArray = var->isArray();
    bool isPointer = var->isPointer();
    bool isChar = var->isChar();

    if (!isActualArray && !isPointer) {
        throw std::runtime_error("Variable is neither an array nor a pointer: " + arrayName);
    }

    index_->EmitRISC(stream, context); // a5

    stream << "mv t2, a5" << std::endl;

    int bytes;
    if (isChar) {
        bytes = 1;
    } else if (var->isDouble() && var->pointerDepth == 0) {
        bytes = 8;
    } else {
        bytes = 4;
    }

    stream << "li t0, " << bytes << std::endl;
    stream << "mul t2, t2, t0" << std::endl;  // t2 = index * bytes

    if (isActualArray) {
        stream << "addi t1, s0, " << var->getStackOffset() << std::endl;
    } else {
        if (context.isGlobalVariable(arrayName)) {
            stream << "la t0, " << arrayName << std::endl;
            stream << "lw t1, 0(t0)" << std::endl;
        } else {
            stream << "lw t1, " << var->getStackOffset() << "(s0)" << std::endl;
        }
    }

    stream << "add t0, t1, t2" << std::endl;  // t0 = base + (index * bytes)

    if (isChar) {
        stream << "lbu a5, 0(t0)" << std::endl;
    } else if (var->isFloat()) {
        stream << "flw fa0, 0(t0)" << std::endl;
    } else if (var->isDouble()) {
        stream << "fld fa0, 0(t0)" << std::endl;
    } else {
        stream << "lw a5, 0(t0)" << std::endl;
    }
}


bool AccessArray::EmitValue(std::ostream& stream, Context& context) const {
    std::string arrayName = array_->getIdentifier();

    auto* var = context.findVariable(arrayName);
    if (!var) {
        throw std::runtime_error("Array/pointer not found: " + arrayName);
    }

    bool isActualArray = var->isArray();
    bool isPointer = var->isPointer();
    bool isChar = var->isChar();

    if (!isActualArray && !isPointer) {
        throw std::runtime_error("Variable is neither an array nor a pointer: " + arrayName);
    }

    index_->EmitRISC(stream, context);

    stream << "    mv t2, a5" << std::endl;

    int bytes;
    if (isChar) {
        bytes = 1;
    } else if (var->isDouble() && var->pointerDepth == 0) {
        bytes = 8;
    } else {
        bytes = 4;
    }

    stream << "    li t1, " << bytes << std::endl;
    stream << "    mul t2, t2, t1" << std::endl;

    if (isActualArray) {
        stream << "    addi t1, s0, " << var->getStackOffset() << std::endl;
    } else {
        if (context.isGlobalVariable(arrayName)) {
            stream << "    la t1, " << arrayName << std::endl;
            stream << "    lw t1, 0(t1)" << std::endl;
        } else {
            stream << "    lw t1, " << var->getStackOffset() << "(s0)" << std::endl;
        }
    }

    stream << "    add t0, t1, t2" << std::endl;

    return true;//t0
}

void AccessArray::Print(std::ostream& stream) const {
    array_->Print(stream);
    stream << "[";
    index_->Print(stream);
    stream << "]";
}


// The goal is to store a value into the array element.
// The expression value is saved to t1, and the index value is directly used in a5.
// The base address is loaded into t0, and the final address is calculated in t0.
void ArrayAssign::EmitRISC(std::ostream& stream, Context& context) const {

    Variable* var = context.findVariable(identifier_);
    if (!var) {
        throw std::runtime_error("Array/pointer not found: " + identifier_);
    }

    bool isActualArray = var->isArray();
    bool isPointer = var->isPointer();
    bool isChar = var->isChar();

    if (!isActualArray && !isPointer) {
        throw std::runtime_error("Variable is neither an array nor a pointer: " + identifier_);
    }

    expression_->EmitRISC(stream, context);

    if (var->isFloat()) {
        stream << "addi sp, sp, -4" << std::endl;
        stream << "fsw fa0, 0(sp)" << std::endl;
    } else if (var->isDouble()) {
        stream << "addi sp, sp, -8" << std::endl;
        stream << "fsd fa0, 0(sp)" << std::endl;
    } else {
        stream << "mv t3, a5" << std::endl;
    }

    index_->EmitRISC(stream, context);

    int bytes;
    if (isChar) {
        bytes = 1;
    } else if (var->isDouble() && var->pointerDepth == 0) {
        bytes = 8;
    } else {
        bytes = 4;
    }

    stream << "li t2, " << bytes << std::endl;
    stream << "mul a5, a5, t2" << std::endl;
    stream << "mv t2, a5" << std::endl;

    if (isActualArray) {
        stream << "addi t1, s0, " << var->getStackOffset() << std::endl;
    } else {
        if (context.isGlobalVariable(identifier_)) {
            stream << "la t0, " << identifier_ << std::endl;
            stream << "lw t1, 0(t0)" << std::endl;
        } else {
            stream << "lw t1, " << var->getStackOffset() << "(s0)" << std::endl;
        }
    }

    stream << "add t0, t1, t2" << std::endl;

    if (var->isFloat()) {
        stream << "flw fa0, 0(sp)" << std::endl;
        stream << "addi sp, sp, 4" << std::endl;
        stream << "fsw fa0, 0(t0)" << std::endl;
    } else if (var->isDouble()) {
        stream << "fld fa0, 0(sp)" << std::endl;
        stream << "addi sp, sp, 8" << std::endl;
        stream << "fsd fa0, 0(t0)" << std::endl;
    } else if (isChar) {
        stream << "sb t3, 0(t0)" << std::endl;
        stream << "mv a5, t3" << std::endl;
    } else {
        stream << "sw t3, 0(t0)" << std::endl;
        stream << "mv a5, t3" << std::endl;
    }
}

void ArrayAssign::Print(std::ostream& stream) const {
    stream << identifier_ << "[";
    index_->Print(stream);
    stream << "] = ";
    expression_->Print(stream);
}

} // namespace ast
