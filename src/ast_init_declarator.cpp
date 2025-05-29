#include "ast_init_declarator.hpp"
#include "ast_variable.hpp"
#include "ast_context.hpp"

namespace ast {

void InitDeclarator::EmitRISC(std::ostream& stream, Context& context) const
{
    // Just handle the initialization if there is one
    if (initializer_) {
        initializer_->EmitRISC(stream, context);
    }
}

void InitDeclarator::Print(std::ostream& stream) const
{
    declarator_->Print(stream);
    if (initializer_) {
        stream << " = ";
        initializer_->Print(stream);
    }
};

}
