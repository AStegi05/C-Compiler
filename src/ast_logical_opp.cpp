#include "ast_logical_opp.hpp"
#include "ast_context.hpp"

namespace ast {

void LogicOp::EmitRISC(std::ostream& stream, Context& context) const {
    // dużo bulu nie chce mis sie łączyć
    bool leftIsFloat = left_->isFloatExpression(context);
    bool rightIsFloat = right_->isFloatExpression(context);
    bool leftIsDouble = left_->isDoubleExpression(context);
    bool rightIsDouble = right_->isDoubleExpression(context);
    bool leftIsChar = left_->isCharExpression(context);
    bool rightIsChar = right_->isCharExpression(context);

    bool resultIsFloat = (leftIsFloat || rightIsFloat) && !leftIsDouble && !rightIsDouble;
    bool resultIsDouble = leftIsDouble || rightIsDouble;
    bool resultIsChar = (leftIsChar || rightIsChar) && !resultIsFloat && !resultIsDouble;

    left_->EmitRISC(stream, context);

    if (resultIsDouble) {
        if (!leftIsDouble) {
            if (leftIsFloat) {
                stream << "fcvt.d.s ft0, fa0" << std::endl;
            } else {
                stream << "fcvt.d.w ft0, a5" << std::endl;
            }
        } else {
            stream << "fmv.d ft0, fa0" << std::endl;
        }

        stream << "addi sp, sp, -8" << std::endl;
        stream << "fsd ft0, 0(sp)" << std::endl;
    }
    else if (resultIsFloat) {
        if (!leftIsFloat) {
            stream << "fcvt.s.w ft0, a5" << std::endl;
        } else {
            stream << "fmv.s ft0, fa0" << std::endl;
        }

        stream << "addi sp, sp, -4" << std::endl;
        stream << "fsw ft0, 0(sp)" << std::endl;
    }
    else {
        stream << "mv t0, a5" << std::endl;
        stream << "addi sp, sp, -4" << std::endl;
        stream << "sw t0, 0(sp)" << std::endl;
    }

    right_->EmitRISC(stream, context);

    if (resultIsDouble) {
        if (!rightIsDouble) {
            if (rightIsFloat) {
                stream << "fcvt.d.s fa0, fa0" << std::endl;
            } else {
                stream << "fcvt.d.w fa0, a5" << std::endl;
            }
        }

        stream << "fld ft1, 0(sp)" << std::endl;
        stream << "addi sp, sp, 8" << std::endl;
    }
    else if (resultIsFloat) {
        if (!rightIsFloat) {
            stream << "fcvt.s.w fa0, a5" << std::endl;
        }

        stream << "flw ft1, 0(sp)" << std::endl;
        stream << "addi sp, sp, 4" << std::endl;
    }
    else {
        stream << "lw t1, 0(sp)" << std::endl;
        stream << "addi sp, sp, 4" << std::endl;
    }

    EmitOp(stream, resultIsFloat, resultIsDouble, resultIsChar, context);
}

void EqOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " == ";
    right_->Print(stream);
    stream << ")";
}

void NotEqOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " != ";
    right_->Print(stream);
    stream << ")";
}

void LessThanOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " < ";
    right_->Print(stream);
    stream << ")";
}

void BiggerThanOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " > ";
    right_->Print(stream);
    stream << ")";
}

void LessThanEqOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " <= ";
    right_->Print(stream);
    stream << ")";
}


void BiggerThanEqOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " >= ";
    right_->Print(stream);
    stream << ")";
}


void LogicalOrOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " || ";
    right_->Print(stream);
    stream << ")";
}

void LogicalAndOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " && ";
    right_->Print(stream);
    stream << ")";
}

void LeftShiftOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " << ";
    right_->Print(stream);
    stream << ")";
}

void RightShiftOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " >> ";
    right_->Print(stream);
    stream << ")";
}

void AndOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " & ";
    right_->Print(stream);
    stream << ")";
}

void OrOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " | ";
    right_->Print(stream);
    stream << ")";
}

void XorOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " ^ ";
    right_->Print(stream);
    stream << ")";
}

// Implementations for unary operators
void LogicalNotOperator::EmitRISC(std::ostream& stream, Context& context) const {
    expr_->EmitRISC(stream, context);
    stream << "seqz a5, a5" << std::endl;
}

void LogicalNotOperator::Print(std::ostream& stream) const {
    stream << "(!";
    expr_->Print(stream);
    stream << ")";
}

void BinaryNotOperator::EmitRISC(std::ostream& stream, Context& context) const {
    expr_->EmitRISC(stream, context);
    stream << "not a5, a5" << std::endl;
}

void BinaryNotOperator::Print(std::ostream& stream) const {
    stream << "(~";
    expr_->Print(stream);
    stream << ")";
}


// Ternary operator implementation
void TenaryOperator::EmitRISC(std::ostream& stream, Context& context) const {
    // Generate labels for jump targets
    std::string falseLabel = context.CreateLabel("ternary_false");
    std::string endLabel = context.CreateLabel("ternary_end");


    // Evaluate condition
    condition_->EmitRISC(stream, context);

    // Jump to false branch if condition is zero
    stream << "beqz a5, " << falseLabel << std::endl;

    // Evaluate true expression
    trueExpr_->EmitRISC(stream, context);

    // Skip false expression
    stream << "j " << endLabel << std::endl;

    // False branch
    stream << falseLabel << ":" << std::endl;
    falseExpr_->EmitRISC(stream, context);


    // End of ternary
    stream << endLabel << ":" << std::endl;
}

void TenaryOperator::Print(std::ostream& stream) const {
    stream << "(";
    condition_->Print(stream);
    stream << " ? ";
    trueExpr_->Print(stream);
    stream << " : ";
    falseExpr_->Print(stream);
    stream << ")";
}

void BitwiseAndAssignOperator::EmitRISC(std::ostream& stream, Context& context) const {
    left_->EmitRISC(stream, context);
    left_->EmitValue(stream, context);
    stream << "mv t1, a5" << std::endl;
    right_->EmitRISC(stream, context);
    stream << "and t1, t1, a5" << std::endl;
    stream << "sw t1, 0(t0)" << std::endl;
}

void BitwiseAndAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " &= ";
    right_->Print(stream);
    stream << ")";
}

void BitwiseOrAssignOperator::EmitRISC(std::ostream& stream, Context& context) const  {
    left_->EmitRISC(stream, context);
    left_->EmitValue(stream, context);
    stream << "mv t1, a5" << std::endl;
    right_->EmitRISC(stream, context);
    stream << "or t1, t1, a5" << std::endl;
    stream << "sw t1, 0(t0)" << std::endl;
}

void BitwiseOrAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " |= ";
    right_->Print(stream);
    stream << ")";
}

void BitwiseXorAssignOperator::EmitRISC(std::ostream& stream, Context& context) const {
    left_->EmitRISC(stream, context);
    left_->EmitValue(stream, context);
    stream << "mv t1, a5" << std::endl;
    right_->EmitRISC(stream, context);
    stream << "xor t1, t1, a5" << std::endl;
    stream << "sw t1, 0(t0)" << std::endl;
}

void BitwiseXorAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " ^= ";
    right_->Print(stream);
    stream << ")";
}

void LeftShiftAssignOperator::EmitRISC(std::ostream& stream, Context& context) const {
    left_->EmitRISC(stream, context);
    left_->EmitValue(stream, context);
    stream << "mv t1, a5" << std::endl;
    right_->EmitRISC(stream, context);
    stream << "sll t1, t1, a5" << std::endl;
    stream << "sw t1, 0(t0)" << std::endl;
}

void LeftShiftAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " <<= ";
    right_->Print(stream);
    stream << ")";
}

void RightShiftAssignOperator::EmitRISC(std::ostream& stream, Context& context) const {
    std::string name = left_->getIdentifier();
    bool isUnsigned = false;

    Variable* var = context.findVariable(name);
    if (var && var->isUnsigned()) {
        isUnsigned = true;
    }

    left_->EmitRISC(stream, context);
    left_->EmitValue(stream, context);
    stream << "mv t1, a5" << std::endl;
    right_->EmitRISC(stream, context);

    if (isUnsigned) {
        stream << "srl t1, t1, a5" << std::endl;
    } else {
        stream << "sra t1, t1, a5" << std::endl;
    }

    stream << "sw t1, 0(t0)" << std::endl;

}

void RightShiftAssignOperator::Print(std::ostream& stream) const {
    stream << "(";
    left_->Print(stream);
    stream << " >>= ";
    right_->Print(stream);
    stream << ")";
}

} // namespace ast
