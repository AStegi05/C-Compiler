#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

class FunctionDeclaration : public Node
{
private:
    const TypeSpecifier declaration_specifiers_;
    NodePtr direct_declarator_;
    NodePtr parameter_list_; // Added missing semicolon

public:
    FunctionDeclaration(TypeSpecifier declaration_specifiers, NodePtr direct_declarator, NodePtr parameter_list)
        : declaration_specifiers_(declaration_specifiers),
          direct_declarator_(std::move(direct_declarator)),
          parameter_list_(std::move(parameter_list)) {} // Ensure proper move semantics

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
