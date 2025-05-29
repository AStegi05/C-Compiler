#include "ast_identifier.hpp"

namespace ast {

void Identifier::EmitRISC(std::ostream&, Context&) const
{
}

void Identifier::Print(std::ostream& stream) const
{
    stream << identifier_;
};

}
