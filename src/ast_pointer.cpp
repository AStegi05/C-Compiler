#include "ast_pointer.hpp"

namespace ast {

    void AddressOf::EmitRISC(std::ostream& stream, Context& context) const {

        if (expression_->EmitValue(stream, context)) {
            stream << "    mv a5, t0" << std::endl;
        } else {
            std::string name = expression_->getIdentifier();
            Variable* var = context.findVariable(name);

            if (context.isGlobalVariable(name)) {
                stream << "    la a5, " << name << std::endl;
            } else {
                stream << "    addi a5, s0, " << var->getStackOffset() << std::endl;
            }
        }
    }

    void AddressOf::Print(std::ostream& stream) const {
        stream << "&";
        expression_->Print(stream);
    }

    // im gonna be honest i have no why this works bc it looks the same, but hands off
    void Dereference::EmitRISC(std::ostream& stream, Context& context) const {

        expression_->EmitRISC(stream, context);

        bool isFloat = expression_->isFloatExpression(context);
        bool isDouble = expression_->isDoubleExpression(context);
        bool isChar = expression_->isCharExpression(context);

        std::string name = expression_->getIdentifier();

        if (isFloat) {
            stream << "flw fa0, 0(a5)" << std::endl;
        } else if (isDouble) {
            stream << "fld fa0, 0(a5)" << std::endl;
        } else if (isChar) {
            stream << "lbu a5, 0(a5)" << std::endl;
        } else {
            stream << "lw a5, 0(a5)" << std::endl;
        }
    }


    void Dereference::Print(std::ostream& stream) const {
        stream << "*";
        expression_->Print(stream);
    }

    bool Dereference::EmitValue(std::ostream& stream, Context& context) const {

        expression_->EmitRISC(stream, context);

        stream << "mv t0, a5" << std::endl;

        return true;
    }


    void AssignPointer::EmitRISC(std::ostream& stream, Context& context) const {

        pointer_->EmitRISC(stream, context);

        stream << "mv t0, a5" << std::endl;

        expression_->EmitRISC(stream, context);

        bool isFloatValue = expression_->isFloatExpression(context);
        bool isDoubleValue = expression_->isDoubleExpression(context);

        if (isFloatValue) {
            stream << "fsw fa0, 0(t0)" << std::endl;
        } else if (isDoubleValue) {
            stream << "fsd fa0, 0(t0)" << std::endl;
        } else {
            stream << "sw a5, 0(t0)" << std::endl;
        }
    }

    void AssignPointer::Print(std::ostream& stream) const {
        stream << "*(";
        pointer_->Print(stream);
        stream << ") = ";
        expression_->Print(stream);
    }

} // namespace ast
