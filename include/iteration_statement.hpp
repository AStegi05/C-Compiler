#include "ast_node.hpp"

namespace ast {

    class WhileStatement : public Node {

        private:
            NodePtr condition_;
            NodePtr body_;

        public:
            WhileStatement(NodePtr condition, NodePtr body)
                : condition_(std::move(condition)),
                  body_(std::move(body)) {}

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;
    };

    class ForStatement : public Node {

        private:
            NodePtr initializer_;
            NodePtr condition_;
            NodePtr update_;
            NodePtr body_;

        public:
            ForStatement(NodePtr initializer, NodePtr condition, NodePtr update, NodePtr body)
                : initializer_(std::move(initializer)),
                  condition_(std::move(condition)),
                  update_(std::move(update)),
                  body_(std::move(body)) {}

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;
    };

} // namespace ast
