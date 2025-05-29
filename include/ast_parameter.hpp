
#pragma once
#include "ast_node.hpp"
#include "ast_context.hpp"
#include "ast_type_specifier.hpp"
#include <string>
#include <vector>

namespace ast {

class ParameterDeclaration : public Node {
private:
    TypeSpecifier type_specifier_;
    NodePtr declarator_;

public:
    ParameterDeclaration(TypeSpecifier type_specifier, NodePtr declarator)
        : type_specifier_(type_specifier),
          declarator_(std::move(declarator)) {}

    TypeSpecifier getTypeSpecifier() const { return type_specifier_; }

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};



} // namespace ast
