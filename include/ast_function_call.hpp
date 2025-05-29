#pragma once

#include "ast_node.hpp"
#include <string>

namespace ast {

class FunctionCall : public Node
{
private:
    std::string functionName_;
    NodePtr arguments_;
public:
    FunctionCall(const std::string& functionName_, NodePtr args)
        : functionName_(functionName_),
          arguments_(std::move(args)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;

};

} // namespace ast
