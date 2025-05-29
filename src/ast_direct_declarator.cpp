#include "ast_direct_declarator.hpp"

namespace ast {

void DirectDeclarator::EmitRISC(std::ostream& stream, Context& context) const
{
    // Process parameters if they exist
    if (parameter_list_) {
        parameter_list_->EmitRISC(stream, context);
    }
}

void DirectDeclarator::Print(std::ostream& stream) const
{
    identifier_->Print(stream);
    stream << "(";
    if (parameter_list_) {
        parameter_list_->Print(stream);
    }
    stream << ")";
}

void PointerDeclarator::EmitRISC(std::ostream& stream, Context& context) const
{
    direct_declarator_->EmitRISC(stream, context);
}

void PointerDeclarator::Print(std::ostream& stream) const {

    for (int i = 0; i < is_pointer_; i++) {
        stream << "*";
    }

    direct_declarator_->Print(stream);
}

} // namespace ast
