#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include <variant>
#include <vector>

namespace ast {

class DeclareArray : public Node {
private:
    NodePtr identifier_;
    int size_;

public:
    DeclareArray(NodePtr identifier, int size)
        : identifier_(std::move(identifier)),
          size_(size) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    std::string getIdentifier() const override {
        return identifier_->getIdentifier();
    }

    int isArray() const override {
        return size_;
    }
};


class InitializeArray : public Node {
private:
    NodePtr elements_;

public:
    InitializeArray(NodePtr elements)
        : elements_(std::move(elements)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    const NodeList* getElements() const { return static_cast<const NodeList*>(elements_.get()); }

    std::vector<std::variant<int, float, double>> getConstList() const override;
};


class AccessArray : public Node {
private:
    NodePtr array_;
    NodePtr index_;

public:
    AccessArray(NodePtr array, NodePtr index)
        : array_(std::move(array)),
          index_(std::move(index)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    std::string getIdentifier() const override { return array_->getIdentifier(); }
    bool EmitValue(std::ostream& stream, Context& context) const override;
    bool isPointerExpression(Context& ) const override { return false; }
};


class ArrayAssign : public Node {
private:
    std::string identifier_;
    NodePtr index_;
    NodePtr expression_;

public:
    ArrayAssign(std::string identifier, NodePtr index, NodePtr expression)
        : identifier_(std::move(identifier)),
          index_(std::move(index)),
          expression_(std::move(expression)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
