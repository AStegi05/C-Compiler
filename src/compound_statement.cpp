#include "compound_statement.hpp"
// moze potem zmienic bo ai uzywam
namespace ast {

void CompoundStatement::EmitRISC(std::ostream& stream, Context& context) const {
    // Create a new scope if needed
    if (create_scope_) {
        context.enterScope();
    }

    // Process statements within this scope
    if (statement_list_) {
        statement_list_->EmitRISC(stream, context);
    }

    // Exit the scope if we created one
    if (create_scope_) {
        context.exitScope();
    }
}

void CompoundStatement::Print(std::ostream& stream) const {
    stream << "{" << std::endl;
    if (statement_list_) {
        statement_list_->Print(stream);
    }
    stream << "}" << std::endl;
}

} // namespace ast
