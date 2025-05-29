#pragma once

#include "ast_node.hpp"
#include "ast_context.hpp"
#include <string>

namespace ast {

class IntConstant : public Node {
    private:
        int value_;
        TypeSpecifier type_;

    public:
        IntConstant(int value, TypeSpecifier type = TypeSpecifier::INT)
         : value_(value),
           type_(type) {}

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

        int getConstantValue() const override { return value_; }
        bool hasConstantValue() const override { return true; }

        bool isChar() const { return type_ == TypeSpecifier::CHAR; }
        bool isCharExpression(Context&) const override {
            return type_ == TypeSpecifier::CHAR;
        }

        bool isUnsigned() const { return type_ == TypeSpecifier::UNSIGNED; }

        std::variant<int, float, double> getConst() const override {
            return value_;
        }

    };

class FloatConstant : public Node{
    private:
        std::variant<float, double> value_;
        TypeSpecifier type_;

    public:
        FloatConstant(double value, TypeSpecifier type = TypeSpecifier::DOUBLE)
            : type_(type) {
                if (type == TypeSpecifier::DOUBLE) {
                    value_ = static_cast<double>(value);
                } else {
                    value_ = static_cast<float>(value);
                }
            }

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

        bool hasConstantFloatValue() const override { return type_ == TypeSpecifier::FLOAT; }
        float getConstantFloatValue() const override {
            return type_ == TypeSpecifier::FLOAT ? std::get<float>(value_) : static_cast<float>(std::get<double>(value_));
        }

        bool hasConstantDoubleValue() const override { return type_ == TypeSpecifier::DOUBLE; }
        double getConstantDoubleValue() const override {
            return type_ == TypeSpecifier::DOUBLE ? std::get<double>(value_) : static_cast<double>(std::get<float>(value_));
        }

        bool isFloatExpression(Context&) const override { return true; }
        bool isDoubleExpression(Context&) const override {
            return type_ == TypeSpecifier::DOUBLE;
        }

        std::variant<int, float, double> getConst() const override {
            if (type_ == TypeSpecifier::DOUBLE) {
                return std::get<double>(value_);
            } else {
                return std::get<float>(value_);
            }
        }
};


} // namespace ast
