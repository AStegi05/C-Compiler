#pragma once

#include "ast_node.hpp"

namespace ast {

class DirectDeclarator : public Node
{
private:
    NodePtr identifier_;
    NodePtr parameter_list_;

public:
    DirectDeclarator(NodePtr identifier, NodePtr parameter_list = nullptr)
    : identifier_(std::move(identifier)),
      parameter_list_(std::move(parameter_list)) {}

    std::string getIdentifier() const override { return identifier_->getIdentifier(); }

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};


class PointerDeclarator : public Node
{
private:
    NodePtr direct_declarator_;
    int is_pointer_;

public:
    PointerDeclarator(NodePtr direct_declarator, int is_pointer)
    : direct_declarator_(std::move(direct_declarator)),
      is_pointer_(is_pointer) {}

    std::string getIdentifier() const override { return direct_declarator_->getIdentifier(); }
    int isPointer() const override { return is_pointer_; }

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
