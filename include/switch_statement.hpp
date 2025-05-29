#include "ast_node.hpp"
#include <iostream>
#include <string>

namespace ast {

class SwitchStatement : public Node {
private:
    NodePtr condition_;
    NodeList case_statement_;

public:
    SwitchStatement(NodePtr condition, NodeList &&case_statement)
        : condition_(std::move(condition)),
          case_statement_(std::move(case_statement)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

class Case : public Node {
private:
    // If condition_ is nullptr, this represents a 'default' case
    NodePtr condition_;
    NodePtr case_statement_;

    // mutable label for two-pass emission
    mutable std::string label_;

public:
    Case(NodePtr condition, NodePtr case_statement)
        : condition_(std::move(condition)),
          case_statement_(std::move(case_statement)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

    // Return raw pointers to avoid copying the unique_pt
    const Node* getCondition() const { return condition_.get(); }
    const Node* getCaseStatement() const { return case_statement_.get(); }

    void setLabel(const std::string &lbl) { label_ = lbl; }
    const std::string &getLabel() const { return label_; }
    bool hasCondition() const { return condition_ != nullptr; }
};

} // namespace ast
