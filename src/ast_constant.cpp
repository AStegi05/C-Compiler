#include "ast_constant.hpp"

namespace ast {

void IntConstant::EmitRISC(std::ostream& stream, Context&) const
{
    stream << "li a5, " << value_ << std::endl;
}

void IntConstant::Print(std::ostream& stream) const
{
    if (type_ == TypeSpecifier::CHAR) {
        stream << "'" << static_cast<char>(value_) << "'";
    } else if (type_ == TypeSpecifier::UNSIGNED) {
        stream << value_ << "U";
    } else {
        stream << value_;
    }
}

void FloatConstant::EmitRISC(std::ostream& stream, Context& context) const
{
    if (type_ == TypeSpecifier::DOUBLE) {
        double dval = std::get<double>(value_);
        std::string label = context.CreateLabel("d_constant");

        stream << ".section .data" << std::endl;
        stream << ".align 3" << std::endl;
        stream << label << ":" << std::endl;
        stream << ".double " << dval << std::endl;
        stream << ".section .text" << std::endl;
        stream << "la t0, " << label << std::endl;
        stream << "fld fa0, 0(t0)" << std::endl;
    } else {
        float val = std::get<float>(value_);

        union {
            float f;
            uint32_t i;
        } convert;
        convert.f = val;

        stream << "li a5, " << convert.i << std::endl;
        stream << "fmv.w.x fa0, a5" << std::endl;
    }
}

void FloatConstant::Print(std::ostream& stream) const
{
    if (type_ == TypeSpecifier::DOUBLE) {
        stream << std::get<double>(value_);
    } else {
        stream << std::get<float>(value_);
    }
}

} // namespace ast
