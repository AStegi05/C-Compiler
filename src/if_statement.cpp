#include "if_statement.hpp"
#include "ast_context.hpp"

namespace ast {

void IfStatement::EmitRISC(std::ostream& stream, Context& context) const
{
std::string else_label = context.CreateLabel("else");
std::string end_label = context.CreateLabel("end_if");

condition_->EmitRISC(stream, context);

stream << "beq a5, zero, " << else_label << std::endl;

if_body_->EmitRISC(stream, context);

stream << "j " << end_label << std::endl;

stream << else_label << ":" << std::endl;

if (else_body_) {
    else_body_->EmitRISC(stream, context);
}

stream << end_label << ":" << std::endl;
}

void IfStatement::Print(std::ostream& stream) const
{
    stream << "If_statement(";
    condition_->Print(stream);
    stream << ", ";
    if_body_->Print(stream);
    stream << ", ";
    if (else_body_) {
        else_body_->Print(stream);
    } else {
        stream << "null";
    }
    stream << ")";
}

} // namespace ast
