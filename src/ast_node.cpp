#include "ast_node.hpp"

namespace ast {

void NodeList::PushBack(NodePtr item)
{
    nodes_.push_back(std::move(item));
}

void NodeList::EmitRISC(std::ostream& stream, Context& context) const
{
    for (const auto& node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->EmitRISC(stream, context);
    }
}

void NodeList::Print(std::ostream& stream) const
{
    for (const auto& node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->Print(stream);
    }
}

// This method retrieves a list of constant values from the array initializer.
// It iterates over the elements of the array (if any) and
// collects their constant values into a vector of variants (which can hold int, float, or double types).
std::vector<std::variant<int, float, double>> NodeList::getConstList() const {
    std::vector<std::variant<int, float, double>> values;

        for (const auto& node : nodes_) {
            if (node) {
                values.push_back(node->getConst());
            }
        }

    return values;
}


// jebać to gówno
std::vector<std::string> NodeList::getEnumNames() const {
    std::vector<std::string> names;

    for (const auto& node : nodes_) {
        if (node) {
            names.push_back(node->getIdentifier());
        }
    }

    return names;
}


}
