#include "ast_string.hpp"
#include "ast_context.hpp"
#include <iostream>
#include <sstream>

namespace ast {

void StringLiteral::EmitRISC(std::ostream& stream, Context& context) const
{
    int index = context.define_string(value_);

    stream << "    lui a5, %hi(string_" << index << ")" << std::endl;
    stream << "    addi a5, a5, %lo(string_" << index << ")" << std::endl;
}

void StringLiteral::Print(std::ostream& stream) const
{
    stream << "\"" << value_ << "\"";
}


void StringEq::EmitRISC(std::ostream& stream, Context& context) const
{
    left_->EmitRISC(stream, context);
    stream << "mv a0, a5" << std::endl;

    right_->EmitRISC(stream, context);
    stream << "mv a1, a5" << std::endl;

    std::string loop_label = context.CreateLabel("strcmp_loop");
    std::string not_equal_label = context.CreateLabel("strcmp_not_equal");
    std::string equal_label = context.CreateLabel("strcmp_equal");
    std::string end_label = context.CreateLabel("strcmp_end");

    stream << loop_label << ":" << std::endl;
    stream << "lb t0, 0(a0)" << std::endl;
    stream << "lb t1, 0(a1)" << std::endl;

    stream << "bne t0, t1, " << not_equal_label << std::endl;
    stream << "beqz t0, " << equal_label << std::endl;

    stream << "addi a0, a0, 1" << std::endl;
    stream << "addi a1, a1, 1" << std::endl;
    stream << "j " << loop_label << std::endl;

    stream << not_equal_label << ":" << std::endl;
    stream << "li a5, 0" << std::endl;
    if (!equal_) {
        stream << "li a5, 1" << std::endl;
    }
    stream << "j " << end_label << std::endl;

    stream << equal_label << ":" << std::endl;
    stream << "li a5, 1" << std::endl;
    if (!equal_) {
        stream << "li a5, 0" << std::endl;
    }

    stream << end_label << ":" << std::endl;
}

void StringEq::Print(std::ostream& stream) const
{
    left_->Print(stream);
    if (equal_) {
        stream << " == ";
    } else {
        stream << " != ";
    }
    right_->Print(stream);
}

} // namespace ast
