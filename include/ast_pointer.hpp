#pragma once

#include "ast_node.hpp"

namespace ast {

    class AddressOf : public Node
    {
    private:
        NodePtr expression_;

    public:
        AddressOf(NodePtr expression)
        : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

        std::string getIdentifier() const override { return expression_->getIdentifier(); }
    };

    class Dereference : public Node
    {
    private:
        NodePtr expression_;

    public:
        Dereference(NodePtr expression)
        : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

        std::string getIdentifier() const override { return expression_->getIdentifier(); }
        int isPointer() const override { return 0; }
        bool EmitValue(std::ostream& stream, Context& context) const override;
        bool isPointerExpression(Context& ) const override { return false; }

    };

    class AssignPointer : public Node {
        private:
            NodePtr pointer_;
            NodePtr expression_;

        public:
            AssignPointer(NodePtr pointer, NodePtr expression)
                : pointer_(std::move(pointer)),
                  expression_(std::move(expression)) {}

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;
        };
}
