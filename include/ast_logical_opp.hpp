#pragma once

#include "ast_node.hpp"
#include "ast_context.hpp"
#include "ast_string.hpp"

namespace ast {

class LogicOp : public Node {
public:
    LogicOp(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;

    // im smoothbrained so these are staying separate
    bool isFloatExpression(Context&) const override { return false; }
    bool isDoubleExpression(Context&) const override { return false; }
    bool isCharExpression(Context&) const override { return false; }
    bool isUnsignedExpression(Context&) const override { return false; }

protected:
    NodePtr left_;
    NodePtr right_;

    virtual void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool isChar, Context& context) const = 0;

    std::string CreateLabel(const std::string& prefix) const {
        static int label_counter = 0;
        return "." + prefix + "_" + std::to_string(label_counter++);
    }


    bool isString(const Node* node, Context& context) const {
        if (!node) return false;

        // keep it dynamic, its one of those bethesda tomato things
        if (dynamic_cast<const StringLiteral*>(node)) { return true; }

        std::string name = node->getIdentifier();

        Variable* var = context.findVariable(name);

        if (var && var->isChar()) {
            return var->isArray() || var->isPointer();
        }

        // for char* results
        return node->isPointerExpression(context) && node->isCharExpression(context);
    }

};


class EqOperator : public LogicOp {
    public:
        EqOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override {

            bool leftIsString = isString(left_.get(), context);
            bool rightIsString = isString(right_.get(), context);

            if (leftIsString && rightIsString) {

                std::string loop_label = CreateLabel("string_comp_loop");
                std::string not_equal_label = CreateLabel("string_comp_not_equal");
                std::string equal_label = CreateLabel("string_comp_equal");
                std::string end_label = CreateLabel("string_comp_end");

                stream << "mv a0, t1" << std::endl;
                stream << "mv a1, a5" << std::endl;

                stream << loop_label << ":" << std::endl;
                stream << "lb t0, 0(a0)" << std::endl;
                stream << "lb t1, 0(a1)" << std::endl;

                stream << "bne t0, t1, " << not_equal_label << std::endl;
                stream << "beqz t0, " << equal_label << std::endl;

                stream << "addi a0, a0, 1" << std::endl;
                stream << "addi a1, a1, 1" << std::endl;
                stream << "j " << loop_label << std::endl;

                stream << not_equal_label << ":" << std::endl;
                stream << "li a5, 0" << std::endl;
                stream << "j " << end_label << std::endl;

                stream << equal_label << ":" << std::endl;
                stream << "li a5, 1" << std::endl;

                stream << end_label << ":" << std::endl;
            } else {
                if (isDouble) {
                    stream << "feq.d a5, ft1, fa0" << std::endl;
                } else if (isFloat) {
                    stream << "feq.s a5, ft1, fa0" << std::endl;
                } else {
                    stream << "xor a5, t1, a5" << std::endl;
                    stream << "seqz a5, a5" << std::endl;
                }
            }
        }
};

class NotEqOperator : public LogicOp {
    public:
        NotEqOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override {

            bool leftIsString = isString(left_.get(), context);
            bool rightIsString = isString(right_.get(), context);

            if (leftIsString && rightIsString) {

                std::string loop_label = CreateLabel("string_comp_loop");
                std::string not_equal_label = CreateLabel("string_comp_not_equal");
                std::string equal_label = CreateLabel("string_comp_equal");
                std::string end_label = CreateLabel("string_comp_end");

                stream << "mv a0, t1" << std::endl;
                stream << "mv a1, a5" << std::endl;

                stream << loop_label << ":" << std::endl;
                stream << "lb t0, 0(a0)" << std::endl;
                stream << "lb t1, 0(a1)" << std::endl;

                stream << "bne t0, t1, " << not_equal_label << std::endl;
                stream << "beqz t0, " << equal_label << std::endl;

                stream << "addi a0, a0, 1" << std::endl;
                stream << "addi a1, a1, 1" << std::endl;
                stream << "j " << loop_label << std::endl;

                stream << not_equal_label << ":" << std::endl;
                stream << "li a5, 1" << std::endl;
                stream << "j " << end_label << std::endl;

                stream << equal_label << ":" << std::endl;
                stream << "li a5, 0" << std::endl;

                stream << end_label << ":" << std::endl;
            } else {
                if (isDouble) {
                    stream << "feq.d a5, ft1, fa0" << std::endl;
                    stream << "xori a5, a5, 1" << std::endl;
                } else if (isFloat) {
                    stream << "feq.s a5, ft1, fa0" << std::endl;
                    stream << "xori a5, a5, 1" << std::endl;
                } else {
                    stream << "xor a5, t1, a5" << std::endl;
                    stream << "snez a5, a5" << std::endl;
                }
            }
        }
};

class LessThanOperator : public LogicOp {
    public:
        LessThanOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override{

            bool isUnsigned = left_->isUnsignedExpression(context) || right_->isUnsignedExpression(context);

            if (isDouble) {
                stream << "flt.d a5, ft1, fa0" << std::endl;
            } else if (isFloat) {
                stream << "flt.s a5, ft1, fa0" << std::endl;
            } else if (isUnsigned) {
                stream << "sltu a5, t1, a5" << std::endl;
            } else {
                stream << "slt a5, t1, a5" << std::endl;
            }
        }
};

class BiggerThanOperator : public LogicOp {
    public:
        BiggerThanOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override {

            bool isUnsigned = left_->isUnsignedExpression(context) || right_->isUnsignedExpression(context);

            if (isDouble) {
                stream << "    flt.d a5, fa0, ft1" << std::endl;
            } else if (isFloat) {
                stream << "    flt.s a5, fa0, ft1" << std::endl;
            } else if (isUnsigned) {
                stream << "    sltu a5, a5, t1" << std::endl;
            } else {
                stream << "    slt a5, a5, t1" << std::endl;
            }
        }
};

class LessThanEqOperator : public LogicOp {
    public:
        LessThanEqOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override {

            bool isUnsigned = left_->isUnsignedExpression(context) || right_->isUnsignedExpression(context);

            if (isDouble) {
                stream << "    fle.d a5, ft1, fa0" << std::endl;
            } else if (isFloat) {
                stream << "    fle.s a5, ft1, fa0" << std::endl;
            } else if (isUnsigned) {
                stream << "    sgtu a5, t1, a5" << std::endl;
                stream << "    xori a5, a5, 1" << std::endl;
            } else {
                stream << "    sgt a5, t1, a5" << std::endl;
                stream << "    xori a5, a5, 1" << std::endl;
                stream << "    andi a5, a5, 0xff" << std::endl;
            }
        }
};

class BiggerThanEqOperator : public LogicOp {
    public:
        BiggerThanEqOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool isFloat, bool isDouble, bool , Context& context) const override {

            bool isUnsigned = left_->isUnsignedExpression(context) || right_->isUnsignedExpression(context);

            if (isDouble) {
                stream << "    fle.d a5, fa0, ft1" << std::endl;
            } else if (isFloat) {
                stream << "    fle.s a5, fa0, ft1" << std::endl;
            } else if (isUnsigned) {
                stream << "    sltu a5, t1, a5" << std::endl;
                stream << "    xori a5, a5, 1" << std::endl;
            } else {
                stream << "    sgt a5, a5, t1" << std::endl;
                stream << "    xori a5, a5, 1" << std::endl;
                stream << "    andi a5, a5, 0xff" << std::endl;
            }
        }
};


class LogicalOrOperator : public LogicOp {
    public:
        LogicalOrOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "    or a5, t1, a5" << std::endl;
            stream << "    snez a5, a5" << std::endl;
        }
};

class LogicalAndOperator : public LogicOp {
    public:
        LogicalAndOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "    snez a5, a5" << std::endl;
            stream << "    snez t1, t1" << std::endl;
            stream << "    and a5, t1, a5" << std::endl;
        }
};


class LogicalNotOperator : public Node {
public:
    LogicalNotOperator(NodePtr expr)
        : expr_(std::move(expr)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    bool isFloatExpression(Context&) const override { return false; }
    bool isDoubleExpression(Context&) const override { return false; }

private:
    NodePtr expr_;
};


class LeftShiftOperator : public LogicOp {
    public:
        LeftShiftOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "    sll a5, t1, a5" << std::endl;
        }
};

class RightShiftOperator : public LogicOp {
    public:
        RightShiftOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& context) const override {

            bool isUnsigned = left_->isUnsignedExpression(context);

            if (isUnsigned) {
                stream << "    srl a5, t1, a5" << std::endl;
            } else {
                stream << "    sra a5, t1, a5" << std::endl;
            }
        }
};

class AndOperator : public LogicOp {
    public:
        AndOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "and a5, t1, a5" << std::endl;
        }
};

class OrOperator : public LogicOp {
    public:
        OrOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "or a5, t1, a5" << std::endl;
        }
};

class XorOperator : public LogicOp {
    public:
        XorOperator(NodePtr left, NodePtr right)
            : LogicOp(std::move(left), std::move(right)) {}

        void Print(std::ostream& stream) const override;

    protected:
        void EmitOp(std::ostream& stream, bool , bool , bool , Context& ) const override {
            stream << "xor a5, t1, a5" << std::endl;
        }
};


class BinaryNotOperator : public Node {
public:
    BinaryNotOperator(NodePtr expr)
        : expr_(std::move(expr)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    bool isFloatExpression(Context&) const override { return false; }
    bool isDoubleExpression(Context&) const override { return false; }

private:
    NodePtr expr_;
};


class TenaryOperator : public Node {
public:
    TenaryOperator(NodePtr condition, NodePtr trueExpr, NodePtr falseExpr)
        : condition_(std::move(condition)),
          trueExpr_(std::move(trueExpr)),
          falseExpr_(std::move(falseExpr)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    // The ternary operator's result type depends on its branches
    bool isFloatExpression(Context& context) const override {
        return trueExpr_->isFloatExpression(context) || falseExpr_->isFloatExpression(context);
    }

    bool isDoubleExpression(Context& context) const override {
        return trueExpr_->isDoubleExpression(context) || falseExpr_->isDoubleExpression(context);
    }

private:
    NodePtr condition_;
    NodePtr trueExpr_;
    NodePtr falseExpr_;
};



class BitwiseAndAssignOperator : public Node {
    public:
        BitwiseAndAssignOperator(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
        NodePtr right_;
    };

class BitwiseOrAssignOperator : public Node  {
    public:
        BitwiseOrAssignOperator(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
        NodePtr right_;
    };


class BitwiseXorAssignOperator : public Node {
    public:
        BitwiseXorAssignOperator(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
        NodePtr right_;
    };

class LeftShiftAssignOperator : public Node {
    public:
        LeftShiftAssignOperator(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
        NodePtr right_;
    };


    class RightShiftAssignOperator : public Node{
    public:
        RightShiftAssignOperator(NodePtr left, NodePtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
        void EmitRISC(std::ostream& stream, Context& context) const override;
        void Print(std::ostream& stream) const override;

    private:
        NodePtr left_;
        NodePtr right_;
    };
} // namespace ast
