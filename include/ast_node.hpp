#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <variant>

#include "ast_context.hpp"
#include "ast_type_specifier.hpp"


namespace ast {

class Node
{
public:
    virtual ~Node() {}
    virtual void EmitRISC(std::ostream& stream, Context& context) const = 0;
    virtual void Print(std::ostream& stream) const = 0;

    virtual std::string getIdentifier() const { return ""; }

    virtual std::variant<int, float, double> getConst() const { return 0; }

    virtual std::vector<std::variant<int, float, double>> getConstList() const { return {}; }

    // IM KILLING MYSELF 3 DAYS ON THIS SHIT KURWA JEBANA NIE CHCE DZIAŁAĆ, TERAZ WIDZE JAKIE TO GÓWNO GODZINE PRZED ODDANIEM
    virtual std::vector<std::string> getEnumNames() const { return {}; }

    //nie dotykaj // i know its a lot
    virtual bool hasConstantValue() const { return false; }
    virtual int getConstantValue() const { return 0; }
    virtual bool hasConstantFloatValue() const { return false; }
    virtual float getConstantFloatValue() const { return 0.0f; }
    virtual bool hasConstantDoubleValue() const { return false; }
    virtual double getConstantDoubleValue() const { return 0.0; }

    virtual int isArray() const { return 0; }

    virtual int isPointer() const { return 0; }
    virtual int getpointerDepth() const { return isPointer(); }

    virtual bool isPointerExpression(Context&) const { return false; }

    virtual bool isCharExpression(Context&) const { return false; }

    virtual bool isUnsignedExpression(Context& context) const {
        std::string name = getIdentifier();
        Variable* var = context.findVariable(name);
        return var && var->isUnsigned();
    }

    virtual bool hasInitializer() const { return false; }
    virtual const Node* getInitializer() const { return nullptr; }
    virtual bool EmitValue(std::ostream&, Context&) const { return false; }

    virtual bool isFloatExpression(Context& ) const { return false; }
    virtual bool isDoubleExpression(Context& ) const { return false; }

};

using NodePtr = std::unique_ptr<const Node>;

class NodeList : public Node
{
private:
    std::vector<NodePtr> nodes_;

public:
    NodeList(NodePtr first_node) { nodes_.push_back(std::move(first_node)); }

    const std::vector<NodePtr>& getNodes() const { return nodes_; }

    void PushBack(NodePtr item);
    virtual void EmitRISC(std::ostream& stream, Context& context) const override;
    virtual void Print(std::ostream& stream) const override;

    virtual std::vector<std::variant<int, float, double>> getConstList() const override;
    virtual std::vector<std::string> getEnumNames() const override;
};

} // namespace ast
