#include "iteration_statement.hpp"

// hope this is robust, seemed kinda easy
namespace ast {

void WhileStatement::EmitRISC(std::ostream& stream, Context& context) const
{
    std::string loop_label = context.CreateLabel(".while_begin");
    std::string continue_label = context.CreateEndStartLabel(".while_continue");
    std::string end_label = context.CreateEndStartLabel(".while_end");
    context.pushLoopLabelContext(end_label, continue_label);

    stream << loop_label << ":" << std::endl;

    condition_->EmitRISC(stream, context);
    stream << "beq a5, zero, " << end_label << std::endl;

    body_->EmitRISC(stream, context);
    stream << continue_label << ":" << std::endl;

    stream << "j " << loop_label << std::endl;
//
    stream << end_label << ":" << std::endl;
}

void WhileStatement::Print(std::ostream& stream) const
{
    stream << "While_statement(";
    condition_->Print(stream);
    stream << ", ";
    body_->Print(stream);
    stream << ")";
}

void ForStatement::EmitRISC(std::ostream& stream, Context& context) const
{
    std::string loop_label = context.CreateLabel(".for_begin");
    std::string continue_label = context.CreateEndStartLabel(".for_continue");
    std::string end_label = context.CreateEndStartLabel(".for_end");
    context.pushLoopLabelContext(end_label, continue_label);

    if (initializer_) {
        initializer_->EmitRISC(stream, context);
    }

    stream << loop_label << ":" << std::endl;

    if (condition_) {
        condition_->EmitRISC(stream, context);
        stream << "beq a5, zero, " << end_label << std::endl;
    }

    body_->EmitRISC(stream, context);

    stream << continue_label << ":" << std::endl;

    if (update_) {
        update_->EmitRISC(stream, context);
    }

    stream << "j " << loop_label << std::endl;

    stream << end_label << ":" << std::endl;
}

void ForStatement::Print(std::ostream& stream) const
{ //idk i was throwing errors
    stream << "For_statement(";
    if (initializer_) {
        initializer_->Print(stream);
    } else {
        stream << "null";
    }
    stream << ", ";
    if (condition_) {
        condition_->Print(stream);
    } else {
        stream << "null";
    }
    stream << ", ";
    if (update_) {
        update_->Print(stream);
    } else {
        stream << "null";
    }
    if(body_){
        stream << ", ";
        body_->Print(stream);
    } else {
        stream << ", null";
    }
}

} // namespace ast
