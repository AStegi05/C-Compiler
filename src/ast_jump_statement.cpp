#include "ast_jump_statement.hpp"
#include "ast_fp.hpp"

namespace ast {

void ReturnStatement::EmitRISC(std::ostream& stream, Context& context) const
{
    if (expression_ != nullptr)
    {
        expression_->EmitRISC(stream, context);
        // Assuming the expression result is placed into a5 (temporary register)
        // Move result into a0 (RISC-V calling convention return register)
         stream << "mv a0, a5" << std::endl;
    }

    // Jump to the function's centralized epilogue
    std::string funcName = context.getCurrentFunctionName();
    stream << "j .L" << funcName << "_epilogue" << std::endl;
}

void ReturnStatement::Print(std::ostream& stream) const
{
    stream << "return";
    if (expression_ != nullptr)
    {
        stream << " ";
        expression_->Print(stream);
    }
    stream << ";" << std::endl;
}

void ContinueStatement::EmitRISC(std::ostream& stream, Context& context) const
{
    const auto* looplabel= context.topLoopLabelContext();
    if (!looplabel) {
        throw std::runtime_error("continue statement used outside of a loop");
    }

    // Jump to the loop's continue label
    stream << "j " << looplabel->startLabel << std::endl;
}

void ContinueStatement::Print(std::ostream& stream) const
{
    stream << "continue;"<<std::endl;
}

void BreakStatement::EmitRISC(std::ostream& stream, Context& context) const {
    // Retrieve the current loop label from context
    const auto* looplabel= context.topLoopLabelContext();
    if (!looplabel) {
        throw std::runtime_error("Break statement used outside of a loop or switch statement");
    }

    // Jump to the loop's end label for a break
    stream << "j " << looplabel->endLabel << std::endl;
}

void BreakStatement::Print(std::ostream& stream) const
{

    stream << "break";
    stream << ";" << std::endl;
}

}
