#include "ast_function_definition.hpp"
#include "ast_fp.hpp"
#include "ast_context.hpp"
#include <sstream>

namespace ast {

void FunctionDefinition::EmitRISC(std::ostream& stream, Context& context) const
{
    context.resetContext();
    std::string funcName = declarator_->getIdentifier();
    context.declaredFunctions.insert(funcName);
    context.setCurrentFunctionName(funcName);

    std::stringstream stringCollection;

    declarator_->EmitRISC(stringCollection, context);

    if (compound_statement_ != nullptr)
    {
        compound_statement_->EmitRISC(stringCollection, context);
    }

    const auto& string_table = context.get_string_table();

    if (!string_table.empty()) {
        stream << ".data" << std::endl;

        for (const auto& entry : string_table) {
            const std::string& str = entry.first;
            int index = entry.second;

            stream << "string_" << index << ":" << std::endl;
            stream << ".string \"" << str << "\"" << std::endl;
        }
    }

    context.resetContext();

    stream << ".text" << std::endl;
    stream << ".globl " << funcName << std::endl;
    stream << ".align 2" << std::endl;
    stream << ".type " << funcName << ", @function" << std::endl;

    stream << funcName << ":" << std::endl;

    int localVarSpace = context.getTotalStackSize();
    setupFramePointer(stream, localVarSpace);

    declarator_->EmitRISC(stream, context);

    if (compound_statement_ != nullptr)
    {
        compound_statement_->EmitRISC(stream, context);
    }

    stream << ".L" << funcName << "_epilogue:" << std::endl;

    restoreFramePointer(stream, localVarSpace);

    stream << "ret" << std::endl;

    stream << ".size " << funcName << ", .-" << funcName << std::endl;
}

void FunctionDefinition::Print(std::ostream& stream) const
{
    stream << declaration_specifiers_ << " ";

    declarator_->Print(stream);
    stream << " {" << std::endl;

    if (compound_statement_ != nullptr)
    {
        compound_statement_->Print(stream);
    }
    stream << "}" << std::endl;
}

}
