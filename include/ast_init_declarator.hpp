#pragma once

#include "ast_node.hpp"
#include <vector>
#include <variant>

namespace ast {

class InitDeclarator : public Node
{
private:
    NodePtr declarator_;
    NodePtr initializer_;

public:
    InitDeclarator(NodePtr declarator, NodePtr initializer)
    : declarator_(std::move(declarator)),
      initializer_(std::move(initializer)) {};

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    std::string getIdentifier() const override { return declarator_->getIdentifier(); }

    int isArray() const override { return declarator_->isArray(); }

    int isPointer() const override { return declarator_->isPointer(); }

    bool hasInitializer() const override { return initializer_ != nullptr; }
    const Node* getInitializer() const { return initializer_.get(); }


    bool hasConstantValue() const override { return initializer_->hasConstantValue(); }
    int getConstantValue() const override { return initializer_->getConstantValue(); }
    // keep the functions separate for clarity bc im retarded
    bool hasConstantFloatValue() const override { return initializer_->hasConstantFloatValue(); }
    float getConstantFloatValue() const override { return initializer_->getConstantFloatValue(); }

    bool hasConstantDoubleValue() const override { return initializer_->hasConstantDoubleValue(); }
    double getConstantDoubleValue() const override { return initializer_->getConstantDoubleValue(); }

    std::vector<std::variant<int, float, double>> getConstList() const override {
        if (initializer_) {
            return initializer_->getConstList();
        }
        return {};
    }

};

} // namespace ast
