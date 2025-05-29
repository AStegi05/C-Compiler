#pragma once

#include "ast_node.hpp"
#include "ast_context.hpp"
#include <string>

namespace ast {


    class VariableDeclare : public Node
    {
    private:
        TypeSpecifier declaration_specifier_;
        NodePtr init_declarator_;

    public:
        VariableDeclare(TypeSpecifier declaration_specifier, NodePtr init_declarator)
            : declaration_specifier_(declaration_specifier),
              init_declarator_(std::move(init_declarator)) {};

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;
    };


    class VariableAssign : public Node
    {
        private:
            std::string identifier_;
            NodePtr expression_;

        public:
            VariableAssign(std::string identifier, NodePtr expression)
            : identifier_(identifier)
            , expression_(std::move(expression)) {};

            void EmitRISC(std::ostream& stream, Context& context) const override;
            void Print(std::ostream& stream) const override;

    };


    class VariableCall : public Node
    {
    private:
        NodePtr init_declarator_;

    public:
        VariableCall(NodePtr init_declarator)
            : init_declarator_(std::move(init_declarator)) {};

        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

        bool EmitValue(std::ostream& stream, Context& context) const override;

        bool isFloatExpression(Context& context) const {
            std::string name = init_declarator_->getIdentifier();
            Variable* var = context.findVariable(name);
            return var && (var->type == TypeSpecifier::FLOAT || var->type == TypeSpecifier::DOUBLE);
        }
        bool isDoubleExpression(Context& context) const override {
            std::string name = init_declarator_->getIdentifier();
            Variable* var = context.findVariable(name);
            return var && var->type == TypeSpecifier::DOUBLE;
        }
        bool isCharExpression(Context& context) const override {
            std::string name = init_declarator_->getIdentifier();
            Variable* var = context.findVariable(name);
            return var && var->isChar();
        }
        bool isPointerExpression(Context& context) const override {
            std::string name = init_declarator_->getIdentifier();
            Variable* var = context.findVariable(name);
            return var && var->isPointer();
        }
        bool isUnsignedExpression(Context& context) const override {
            std::string name = init_declarator_->getIdentifier();
            Variable* var = context.findVariable(name);
            return var && var->isUnsigned();
        }

    };

} // namespace ast
