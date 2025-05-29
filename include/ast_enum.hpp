
#include "ast_node.hpp"
#include "ast_context.hpp"
#include <string>
#include <vector>
#include <variant>

namespace ast {

//  Represents a single enum value in an enum declaration.
//  For example, in "enum Color { RED = 1, GREEN, BLUE };",
//  RED, GREEN, and BLUE are Enumerator objects.
class Enumerator : public Node {
private:
    // The name of the enumerator (e.g., "RED")
    NodePtr identifier_;

    // Optional explicit value expression (e.g., "= 1")
    NodePtr value_;

public:
    Enumerator(NodePtr identifier, NodePtr value)
        : identifier_(std::move(identifier)),
          value_(std::move(value)) {}

    // Get the identifier name
    std::string getIdentifier() const override { return identifier_->getIdentifier(); }

    // Get the constant value if explicitly assigned
    std::variant<int, float, double> getConst() const override {
        if (value_) {
            return value_->getConst();
        }
        return 0; // Default value if not specified
    }

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};


// Represents an enum declaration.
// For example: "enum Color { RED, GREEN, BLUE };"
class EnumSpecifier : public Node {
private:
    // Optional name of the enum type (e.g., "Color")
    NodePtr identifier_;

    // List of enum values (RED, GREEN, BLUE)
    NodePtr members_;

public:
    EnumSpecifier(NodePtr identifier, NodePtr members)
        : identifier_(std::move(identifier)),
          members_(std::move(members)) {}

    // Get the enum type name if present
    std::string getIdentifier() const override {
        if (identifier_) {
            return identifier_->getIdentifier();
        }
        return ""; // Anonymous enum
    }

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast

