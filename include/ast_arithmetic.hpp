#pragma once

#include "ast_node.hpp"
#include "ast_array.hpp"
#include <string>

namespace ast {

class ArithmeticOp : public Node {
    public:
        ArithmeticOp(NodePtr left, NodePtr right)
            : left_(std::move(left)),
              right_(std::move(right)) {}

        void EmitRISC(std::ostream& stream, Context& context) const override;

        bool isFloatExpression(Context& context) const override {
            return left_->isFloatExpression(context) || right_->isFloatExpression(context);
        }

        bool isDoubleExpression(Context& context) const override {
            return left_->isDoubleExpression(context) || right_->isDoubleExpression(context);
        }

        bool isCharExpression(Context& context) const override {
            return left_->isCharExpression(context) || right_->isCharExpression(context);
        }

        bool isUnsignedExpression(Context& context) const override {
            return left_->isUnsignedExpression(context) || right_->isUnsignedExpression(context);
        }

    protected:
        NodePtr left_;
        NodePtr right_;

        virtual void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool isChar, bool isUnsigned) const = 0;

        bool isPointerOp(const Node* node, Context& context) const {

            std::string name = node->getIdentifier();

            auto var = context.findVariable(name);

            if (var && var->isPointer()) {
                return true;
            }

            if (node->isPointer() > 0) {
                return true;
            }

            return false;
        }
};

class AddExpression : public ArithmeticOp {
    public:
        AddExpression(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const;
};

class SubExpression : public ArithmeticOp {
    public:
        SubExpression(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const;
};

class MultExpression : public ArithmeticOp {
    public:
        MultExpression(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context&, bool isFloat, bool isDouble, bool, bool) const {
            if (isDouble) {
                stream << "    fmul.d fa0, ft1, fa0" << std::endl;
            } else if (isFloat) {
                stream << "    fmul.s fa0, ft1, fa0" << std::endl;
            } else {
                stream << "    mul a5, t1, a5" << std::endl;
            }
        }
};

class DivExpression : public ArithmeticOp {
    public:
        DivExpression(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context&, bool isFloat, bool isDouble, bool, bool isUnsigned) const {
            if (isDouble) {
                stream << "    fdiv.d fa0, ft1, fa0" << std::endl;
            } else if (isFloat) {
                stream << "    fdiv.s fa0, ft1, fa0" << std::endl;
            } else {
                if (isUnsigned) {
                    stream << "    divu a5, t1, a5" << std::endl;
                } else {
                    stream << "    div a5, t1, a5" << std::endl;
                }
            }
        }
};

class ModExpression : public ArithmeticOp {
public:
    ModExpression(NodePtr left, NodePtr right)
        : ArithmeticOp(std::move(left), std::move(right)) {}

    void Print(std::ostream& stream) const override;

protected:
    void EmitOp(std::ostream& stream, Context&, bool, bool, bool, bool isUnsigned) const override {
        if (isUnsigned) {
            stream << "    remu a5, t1, a5" << std::endl;
        } else {
            stream << "    rem a5, t1, a5" << std::endl;
        }
    }
};

class Inc_Opp : public Node {
    public:
        Inc_Opp(NodePtr left)
            : left_(std::move(left)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
};

class Dec_Opp : public Node {
    public:
        Dec_Opp(NodePtr left)
            : left_(std::move(left)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
};

class PlusAssignOperator : public ArithmeticOp {
    public:
        PlusAssignOperator(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {
            if (isDouble) {
                stream << "    fadd.d fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsd fa0, 0(t0)" << std::endl;
            } else if (isFloat) {
                stream << "    fadd.s fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsw fa0, 0(t0)" << std::endl;
            } else {
                stream << "    add t1, t1, a5" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    sw t1, 0(t0)" << std::endl;
            }
        }
};


class MinusAssignOperator : public ArithmeticOp {
    public:
        MinusAssignOperator(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {
            if (isDouble) {
                stream << "    fsub.d fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsd fa0, 0(t0)" << std::endl;
            } else if (isFloat) {
                stream << "    fsub.s fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsw fa0, 0(t0)" << std::endl;
            } else {
                stream << "    sub t1, t1, a5" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    sw t1, 0(t0)" << std::endl;
            }
        }
};


class MultiplyAssignOperator : public ArithmeticOp {
    public:
        MultiplyAssignOperator(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {
            if (isDouble) {
                stream << "    fmul.d fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsd fa0, 0(t0)" << std::endl;
            } else if (isFloat) {
                stream << "    fmul.s fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsw fa0, 0(t0)" << std::endl;
            } else {
                left_->EmitValue(stream, context);
                stream << "    mul t1, t1, a5" << std::endl;
                stream << "    sw t1, 0(t0)" << std::endl;
            }
        }
};


class DivideAssignOperator : public ArithmeticOp {
    public:
        DivideAssignOperator(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool isFloat, bool isDouble, bool, bool) const {
            if (isDouble) {
                stream << "    fdiv.d fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsd fa0, 0(t0)" << std::endl;
            } else if (isFloat) {
                stream << "    fdiv.s fa0, ft1, fa0" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    fsw fa0, 0(t0)" << std::endl;
            } else {
                stream << "    div t1, t1, a5" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    sw t1, 0(t0)" << std::endl;
            }
        }
};

class ModuloAssignOperator : public ArithmeticOp {
    public:
        ModuloAssignOperator(NodePtr left, NodePtr right)
            : ArithmeticOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, Context& context, bool, bool, bool, bool) const override {

                stream << "    rem t1, t1, a5" << std::endl;
                left_->EmitValue(stream, context);
                stream << "    sw t1, 0(t0)" << std::endl;

        }
};

class NegateOperator : public Node{
    public:
    NegateOperator(NodePtr left)
        : left_(std::move(left)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
};

class PosOperator : public Node{
    public:
        PosOperator(NodePtr left)
        : left_(std::move(left)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
};

} // namespace ast
