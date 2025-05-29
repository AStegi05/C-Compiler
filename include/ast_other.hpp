#pragma once

#include "ast_node.hpp"
#include "ast_context.hpp"
#include "ast_type_specifier.hpp"
#include <variant>
#include <vector>
#include <string>

namespace ast {
class SizeoffExprFunction : public Node
{
private:
    NodePtr Expr_;
public:
    SizeoffExprFunction(NodePtr Expr)
    : Expr_(std::move(Expr)){}
    std::string name  = Expr_->getIdentifier();

    std::string getIdentifier() const override {
        return Expr_->getIdentifier();
    }

    bool isFloatExpression(Context& context) const override {
        // If either operand is a float, the result is a float
        return Expr_->isFloatExpression(context);
    }

    bool isDoubleExpression(Context& context) const override {
        // If either operand is a double, the result is a double
        return Expr_->isDoubleExpression(context);
    }

    bool isCharExpression(Context& context) const override {
        // Check if either operand is a char
        return Expr_->isCharExpression(context);
    }
    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

class SizeoffTypeFunction : public Node
{
private:
    NodePtr Type_;
public:
    SizeoffTypeFunction(NodePtr Type)
    : Type_(std::move(Type)){}



    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
