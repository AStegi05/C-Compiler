#pragma once
// moze potem zmienic bo ai uzywam
#include "ast_node.hpp"
#include "ast_context.hpp"

namespace ast {

class CompoundStatement : public Node {
    private:
        NodePtr statement_list_;
        bool create_scope_; // Flag to determine if this compound statement creates a new scope

    public:
        // Constructor for compound statements that create a new scope
        CompoundStatement(NodePtr statement_list, bool create_scope = true)
            : statement_list_(std::move(statement_list)),
            create_scope_(create_scope) {}

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

};

} // namespace ast
