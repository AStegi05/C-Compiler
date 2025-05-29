#include "ast_function_call.hpp"
#include "ast_fp.hpp"
#include "ast_context.hpp"
#include <string>
#include <iostream>

namespace ast {


    void FunctionCall::EmitRISC(std::ostream& stream, Context& context) const
    {
        if (functionName_.empty()) {
            throw std::runtime_error("Function name is empty in FunctionCall::EmitRISC");
        }

        const NodeList* argList = dynamic_cast<const NodeList*>(arguments_.get());
        if (argList) {
            const auto& nodes = argList->getNodes();
            size_t argCount = nodes.size();

            for (size_t i = 0; i < argCount; ++i) {
                if (!nodes[i]) continue;

                nodes[i]->EmitRISC(stream, context);  // Compute argument expression

                // Assign first 8 arguments to a0-a7
                stream << "    mv a" << i << ", a5" << std::endl;

                // Debug: Print assigned argument values
                std::cerr << "DEBUG: Argument " << i << " assigned to register t" << i << std::endl;
            }
        }

        // Emit function call
        stream << "    call " << functionName_ << std::endl;
        std::cerr << "DEBUG: Function " << functionName_ << " called" << std::endl;
    }




    void FunctionCall::Print(std::ostream& stream) const
    {
        stream << functionName_ << "()";
    }
}
