#include "ast_arithmetic.hpp"
#include "ast_context.hpp"

namespace ast {

void ArithmeticOp::EmitRISC(std::ostream& stream, Context& context) const {
    bool leftIsFloat = left_->isFloatExpression(context);
    bool rightIsFloat = right_->isFloatExpression(context);
    bool leftIsDouble = left_->isDoubleExpression(context);
    bool rightIsDouble = right_->isDoubleExpression(context);
    bool leftIsChar = left_->isCharExpression(context);
    bool rightIsChar = right_->isCharExpression(context);
    bool leftIsPointer = isPointerOp(left_.get(), context);
    // bool rightIsPointer = isPointerOp(right_.get(), context);
    bool leftIsUnsigned = left_->isUnsignedExpression(context);
    bool rightIsUnsigned = right_->isUnsignedExpression(context);

    bool resultIsFloat = leftIsFloat || rightIsFloat;
    bool resultIsDouble = leftIsDouble || rightIsDouble;
    bool resultIsChar = (leftIsChar || rightIsChar) && !resultIsFloat && !resultIsDouble;
    // bool resultIsPointer = leftIsPointer || rightIsPointer;

    bool resultIsUnsigned = leftIsUnsigned || rightIsUnsigned;

    left_->EmitRISC(stream, context);

    if (leftIsPointer) {
        stream << "    mv t1, a5" << std::endl;
        stream << "    addi sp, sp, -4" << std::endl;
        stream << "    sw t1, 0(sp)" << std::endl;
    }

    if (resultIsDouble) {

        if (!leftIsDouble) {

            if (leftIsFloat) {
                stream << "    fcvt.d.s ft0, fa0" << std::endl;
            } else {
                stream << "    fcvt.d.w ft0, a5" << std::endl;
            }

        } else {
            stream << "    fmv.d ft0, fa0" << std::endl;
        }

        stream << "    addi sp, sp, -8" << std::endl;
        stream << "    fsd ft0, 0(sp)" << std::endl;
    }
    else if (resultIsFloat) {

        if (!leftIsFloat) {
            stream << "    fcvt.s.w ft0, a5" << std::endl;
        } else {
            stream << "    fmv.s ft0, fa0" << std::endl;
        }

        stream << "    addi sp, sp, -4" << std::endl;
        stream << "    fsw ft0, 0(sp)" << std::endl;
    }
    else {
        stream << "    mv t0, a5" << std::endl;
        stream << "    addi sp, sp, -4" << std::endl;
        stream << "    sw t0, 0(sp)" << std::endl;
    }

    right_->EmitRISC(stream, context);

    if (leftIsPointer) {
        stream << "    lw t1, 0(sp)" << std::endl;
        stream << "    addi sp, sp, 4" << std::endl;
    }

    if (resultIsDouble) {

        if (!rightIsDouble) {

            if (rightIsFloat) {
                stream << "    fcvt.d.s fa0, fa0" << std::endl;
            } else {
                stream << "    fcvt.d.w fa0, a5" << std::endl;
            }
        }

        stream << "    fld ft1, 0(sp)" << std::endl;
        stream << "    addi sp, sp, 8" << std::endl;

    } else if (resultIsFloat) {

        if (!rightIsFloat) {
            stream << "    fcvt.s.w fa0, a5" << std::endl;
        }

        stream << "    flw ft1, 0(sp)" << std::endl;
        stream << "    addi sp, sp, 4" << std::endl;
    }
    else {
        stream << "    lw t1, 0(sp)" << std::endl;
        stream << "    addi sp, sp, 4" << std::endl;
    }


    EmitOp(stream, context, resultIsFloat, resultIsDouble, resultIsChar, resultIsUnsigned);


}

void AddExpression::EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {

    bool leftIsChar = left_->isCharExpression(context);
    bool rightIsChar = right_->isCharExpression(context);
    bool leftIsPointer = left_->isPointerExpression(context);
    bool rightIsPointer = right_->isPointerExpression(context);


    int bytes = 4; // Default


    if (leftIsPointer && leftIsChar) {
        bytes = 1;
    } else if (rightIsPointer && rightIsChar) {
        bytes = 1;
    }

    std::string leftName = left_->getIdentifier();
    std::string rightName = right_->getIdentifier();

    if (leftIsPointer && !leftName.empty()) {

        auto var = context.findVariable(leftName);

        if (var && var->isPointer()) {
            if (var->isChar()) {
                bytes = 1;
            } else if (var->isDouble()) {
                bytes = 8;
            }
        }
    }
    else if (rightIsPointer && !rightName.empty()) {

        auto var = context.findVariable(rightName);

        if (var && var->isPointer()) {
            if (var->isChar()) {
                bytes = 1;
            } else if (var->isDouble()) {
                bytes = 8;
            }
        }
    }

    if (leftIsPointer) {
        stream << "li t2, " << bytes << std::endl;
        stream << "mul a5, a5, t2" << std::endl;
        stream << "add a5, t1, a5" << std::endl;
    }
    else if (rightIsPointer) {
        stream << "li t2, " << bytes << std::endl;
        stream << "mul t1, t1, t2" << std::endl;
        stream << "add a5, t1, a5" << std::endl;
    }
    else if (isDouble) {
        stream << "fadd.d fa0, ft1, fa0" << std::endl;
    }
    else if (isFloat) {
        stream << "fadd.s fa0, ft1, fa0" << std::endl;
    }
    else {
        stream << "add a5, t1, a5" << std::endl;
    }
}

void AddExpression::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " + ";
    right_->Print(stream);
    stream << ")";
}

void SubExpression::EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {

    bool leftIsChar = left_->isCharExpression(context);
    bool rightIsChar = right_->isCharExpression(context);
    bool leftIsPointer = left_->isPointerExpression(context);
    bool rightIsPointer = right_->isPointerExpression(context);

    int bytes = 4;

    if (leftIsPointer && leftIsChar) {
        bytes = 1;
    }


    if (leftIsPointer && rightIsPointer) {
        if (leftIsChar && rightIsChar) {
            bytes = 1;
        }
    }

    std::string leftName = left_->getIdentifier();
    std::string rightName = right_->getIdentifier();

    if (leftIsPointer && !leftName.empty()) {

        auto var = context.findVariable(leftName);

        if (var && var->isPointer()) {
            if (var->isChar()) {
                bytes = 1;
            } else if (var->isDouble()) {
                bytes = 8;
            }
        }
    }


    if (leftIsPointer && rightIsPointer && !leftName.empty() && !rightName.empty()) {
        auto leftVar = context.findVariable(leftName);
        auto rightVar = context.findVariable(rightName);

        if (leftVar && rightVar && leftVar->isPointer() && rightVar->isPointer()) {
            if (leftVar->isChar() && rightVar->isChar()) {
                bytes = 1;
            } else if (leftVar->isDouble() && rightVar->isDouble()) {
                bytes = 8;
            }
        }
    }

    if (leftIsPointer && !rightIsPointer) {
        stream << "li t2, " << bytes << std::endl;
        stream << "mul a5, a5, t2" << std::endl;
        stream << "sub a5, t1, a5" << std::endl;
    }
    else if (leftIsPointer && rightIsPointer) {
        stream << "sub a5, t1, a5" << std::endl;
        stream << "li t2, " << bytes << std::endl;
        stream << "div a5, a5, t2" << std::endl;
    }
    else if (isDouble) {
        stream << "fsub.d fa0, ft1, fa0" << std::endl;
    }
    else if (isFloat) {
        stream << "fsub.s fa0, ft1, fa0" << std::endl;
    }
    else {
        stream << "sub a5, t1, a5" << std::endl;
    }
}

void SubExpression::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " - ";
    right_->Print(stream);
    stream << ")";
}


void MultExpression::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " * ";
    right_->Print(stream);
    stream << ")";
}


void DivExpression::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " / ";
    right_->Print(stream);
    stream << ")";
}


void ModExpression::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " % ";
    right_->Print(stream);
    stream << ")";
}

void Inc_Opp::EmitRISC(std::ostream& stream, Context& context) const {

    bool isFloat = left_->isFloatExpression(context);
    bool isDouble = left_->isDoubleExpression(context);
    bool isChar = left_->isCharExpression(context);

    std::string name = left_->getIdentifier();
    bool isPointer = false;
    int bytes = 4;


    auto var = context.findVariable(name);

    if (var && var->isPointer()) {

        isPointer = true;

        if (var->isDouble()) {
            bytes = 8;
        }
    }


    if (left_->EmitValue(stream, context)) { // t0

        if (isPointer) {
            stream << "lw a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, " << bytes << std::endl;
            stream << "sw a5, 0(t0)" << std::endl;
        }
        else if (isDouble) {
            stream << "fld fa0, 0(t0)" << std::endl;
            stream << "li a1, 0x3ff00000" << std::endl;  // High 32 bits
            stream << "li a2, 0x00000000" << std::endl;  // Low 32 bits
            stream << "addi sp, sp, -8" << std::endl;
            stream << "sw a2, 0(sp)" << std::endl;
            stream << "sw a1, 4(sp)" << std::endl;
            stream << "fld ft1, 0(sp)" << std::endl;
            stream << "addi sp, sp, 8" << std::endl;
            stream << "fadd.d fa0, fa0, ft1" << std::endl;
            stream << "fsd fa0, 0(t0)" << std::endl;
        } else if (isFloat) {
            stream << "flw fa0, 0(t0)" << std::endl;
            stream << "li a1, 0x3f800000" << std::endl; // 1.0 in IEEE 754 format
            stream << "fmv.w.x ft1, a1" << std::endl;
            stream << "fadd.s fa0, fa0, ft1" << std::endl;
            stream << "fsw fa0, 0(t0)" << std::endl;
        } else if (isChar) {
            stream << "lbu a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, 1" << std::endl;
            stream << "andi a5, a5, 0xFF" << std::endl;
            stream << "sb a5, 0(t0)" << std::endl;
        } else {
            stream << "lw a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, 1" << std::endl;
            stream << "sw a5, 0(t0)" << std::endl;
        }
    } else {
        left_->EmitRISC(stream, context);

        if (isPointer) {
            stream << "addi a5, a5, " << bytes << std::endl;
        }
        else if (isDouble) {
            stream << "li a1, 0x3ff00000" << std::endl;  // High 32 bits
            stream << "li a2, 0x00000000" << std::endl;  // Low 32 bits
            stream << "addi sp, sp, -8" << std::endl;
            stream << "sw a2, 0(sp)" << std::endl;
            stream << "sw a1, 4(sp)" << std::endl;
            stream << "fld ft1, 0(sp)" << std::endl;
            stream << "addi sp, sp, 8" << std::endl;
            stream << "fadd.d fa0, fa0, ft1" << std::endl;
        } else if (isFloat) {
            stream << "li a1, 0x3f800000" << std::endl;  // 1.0 in IEEE 754 format
            stream << "fmv.w.x ft1, a1" << std::endl;
            stream << "fadd.s fa0, fa0, ft1" << std::endl;
        } else if (isChar) {
            stream << "addi a5, a5, 1" << std::endl;
            stream << "andi a5, a5, 0xFF" << std::endl;
        } else {
            stream << "addi a5, a5, 1" << std::endl;
        }
    }
}

void Inc_Opp::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " ++ ";
    stream << ")";
}


void Dec_Opp::EmitRISC(std::ostream& stream, Context& context) const {

    bool isFloat = left_->isFloatExpression(context);
    bool isDouble = left_->isDoubleExpression(context);
    bool isChar = left_->isCharExpression(context);

    std::string name = left_->getIdentifier();
    bool isPointer = false;
    int bytes = 4;

    auto var = context.findVariable(name);

    if (var && var->isPointer()) {
        isPointer = true;


        if (var->isDouble()) {
            bytes = 8;
        }
    }

    if (left_->EmitValue(stream, context)) {

        if (isPointer) {
            stream << "lw a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, -" << bytes << std::endl;
            stream << "sw a5, 0(t0)" << std::endl;
        }
        else if (isDouble) {
            stream << "fld fa0, 0(t0)" << std::endl;
            stream << "li a1, 0x3ff00000" << std::endl;
            stream << "li a2, 0x00000000" << std::endl;
            stream << "addi sp, sp, -8" << std::endl;
            stream << "sw a2, 0(sp)" << std::endl;
            stream << "sw a1, 4(sp)" << std::endl;
            stream << "fld ft1, 0(sp)" << std::endl;
            stream << "addi sp, sp, 8" << std::endl;
            stream << "fsub.d fa0, fa0, ft1" << std::endl;
            stream << "fsd fa0, 0(t0)" << std::endl;
        } else if (isFloat) {
            stream << "flw fa0, 0(t0)" << std::endl;
            stream << "li a1, 0x3f800000" << std::endl;
            stream << "fmv.w.x ft1, a1" << std::endl;
            stream << "fsub.s fa0, fa0, ft1" << std::endl;
            stream << "fsw fa0, 0(t0)" << std::endl;
        } else if (isChar) {
            stream << "lbu a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, -1" << std::endl;
            stream << "andi a5, a5, 0xFF" << std::endl;
            stream << "sb a5, 0(t0)" << std::endl;
        } else {
            stream << "lw a5, 0(t0)" << std::endl;
            stream << "addi a5, a5, -1" << std::endl;
            stream << "sw a5, 0(t0)" << std::endl;
        }
    } else {
        left_->EmitRISC(stream, context);

        if (isPointer) {
            stream << "addi a5, a5, -" << bytes << std::endl;
        }
        else if (isDouble) {
            stream << "li a1, 0x3ff00000" << std::endl;
            stream << "li a2, 0x00000000" << std::endl;
            stream << "addi sp, sp, -8" << std::endl;
            stream << "sw a2, 0(sp)" << std::endl;
            stream << "sw a1, 4(sp)" << std::endl;
            stream << "fld ft1, 0(sp)" << std::endl;
            stream << "addi sp, sp, 8" << std::endl;
            stream << "fsub.d fa0, fa0, ft1" << std::endl;
        } else if (isFloat) {
            stream << "li a1, 0x3f800000" << std::endl;
            stream << "fmv.w.x ft1, a1" << std::endl;
            stream << "fsub.s fa0, fa0, ft1" << std::endl;
        } else if (isChar) {
            stream << "addi a5, a5, -1" << std::endl;
            stream << "andi a5, a5, 0xFF" << std::endl;
        } else {
            stream << "addi a5, a5, -1" << std::endl;
        }
    }
}

void Dec_Opp::Print(std::ostream& stream) const
{
    stream << "(";
    left_->Print(stream);
    stream << " -- ";
    stream << ")";
}



void PlusAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " += ";
    right_->Print(stream);
    stream << ")";
}



void MinusAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " -= ";
    right_->Print(stream);
    stream << ")";
}



void MultiplyAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " *= ";
    right_->Print(stream);
    stream << ")";
}



void DivideAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " /= ";
    right_->Print(stream);
    stream << ")";
}



void ModuloAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " %= ";
    right_->Print(stream);
    stream << ")";
}

void PosOperator::EmitRISC(std::ostream& stream, Context& context) const {
    left_->EmitRISC(stream, context);
    stream << "add a5, x0, a5" << std::endl;
}

void PosOperator::Print(std::ostream& stream) const {
    stream << "(";
    stream << " + ";
    left_->Print(stream);
    stream << ")";
}

void NegateOperator::EmitRISC(std::ostream& stream, Context& context) const {
    left_->EmitRISC(stream, context);
    stream << "sub a5, x0, a5" << std::endl;
}

void NegateOperator::Print(std::ostream& stream) const {
    stream << "(";
    stream << " - ";
    left_->Print(stream);
    stream << ")";
}

} // namespace ast
