#pragma once

#include "ast_node.hpp"
#include <string>

namespace ast {

    class StringLiteral : public Node {
        private:
            std::string value_;

        public:
            StringLiteral(std::string value)
                : value_(std::move(value)) {}

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;

            const std::string& getValue() const { return value_; }

            bool isPointerExpression(Context&) const override { return true; }
            bool isCharExpression(Context&) const override { return true; }
    };


    class StringEq : public Node {
        private:
            NodePtr left_;
            NodePtr right_;
            bool equal_;

        public:
            StringEq(NodePtr left, NodePtr right, bool equal)
                : left_(std::move(left)),
                  right_(std::move(right)),
                  equal_(equal) {}

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;
    };

} // namespace ast
