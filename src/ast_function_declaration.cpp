#include "ast_function_declaration.hpp"
#include "ast_fp.hpp"
#include "ast_context.hpp"

namespace ast {

void FunctionDeclaration::EmitRISC(std::ostream& stream, Context& context) const
{
    if (!direct_declarator_) {
        throw std::runtime_error("Function declaration missing direct declarator");
    }

    std::string funcName = direct_declarator_->getIdentifier(); // Fix the function name retrieval
    context.declaredFunctions.insert(funcName); // Function declarations should be tracked but not stored as variables

    stream << "# Function Declaration: " << funcName << std::endl;

    if (parameter_list_) {
        parameter_list_->EmitRISC(stream, context);
    }
}

void FunctionDeclaration::Print(std::ostream& stream) const
{
    stream << declaration_specifiers_ << " ";

    if (direct_declarator_) {
        direct_declarator_->Print(stream);
    }

    stream << "(";

    if (parameter_list_) {
        parameter_list_->Print(stream);
    }

    stream << ");" << std::endl;
}

} // namespace ast
