#include "switch_statement.hpp"
#include "ast_context.hpp"
#include <iostream>
#include <string>

namespace ast {

void SwitchStatement::EmitRISC(std::ostream& stream, Context& context) const {
    // Create a label for the end of the switch statement
    std::string end_label = context.CreateEndStartLabel(".switch_end");
    std::string loop_label = context.CreateEndStartLabel(".switch_begin");
    context.pushLoopLabelContext(end_label, loop_label);
    // Emit the switch expression
    condition_->EmitRISC(stream, context);
    stream << "mv t0, a5" << std::endl;

    // First pass: Emit all case condition checks
    for (const auto &node : case_statement_.getNodes()) {
        if (!node)
            continue;
        // Get a pointer to the Case node
        const Case *constCaseNode = static_cast<const Case*>(node.get());
        Case *caseNode = const_cast<Case*>(constCaseNode);
        if (caseNode->hasCondition()) {
            // Create and store a unique label for this case.
            std::string caseLabel = context.CreateLabel(".Case");
            caseNode->setLabel(caseLabel);
            stream << "# Case condition:" << std::endl;
            // Emit the case’s condition
            caseNode->getCondition()->EmitRISC(stream, context);
            stream << "beq t0, a5, " << caseLabel << std::endl;
        } else {
            // Default case: assign a label without a branch check
            std::string defaultLabel = context.CreateLabel("Default");
            caseNode->setLabel(defaultLabel);
        }
    }
    stream<< "j " << end_label<<std::endl;

    // Second pass: Emit all case bodies
    for (const auto &node : case_statement_.getNodes()) {
        if (!node)
            continue;
        const Case *constCaseNode = static_cast<const Case*>(node.get());
        Case *caseNode = const_cast<Case*>(constCaseNode);
        stream << caseNode->getLabel() <<":"<< std::endl;
        if (caseNode->getCaseStatement())
            caseNode->getCaseStatement()->EmitRISC(stream, context);
    }

    stream << end_label << ":" << std::endl;
    stream << "# End switch statement" << std::endl;
}

void SwitchStatement::Print(std::ostream& stream) const {
    stream << "switch(";
    condition_->Print(stream);
    stream << ")";
    case_statement_.Print(stream);
}

void Case::EmitRISC(std::ostream& stream, Context& context) const {
    // Parameters unused since SwitchStatement::EmitRISC handles both condition and body emission
    (void)stream;
    (void)context;
}

void Case::Print(std::ostream& stream) const {
    if (condition_) {
        stream << "case ";
        condition_->Print(stream);
        stream << ":";
    } else {
        stream << "default:";
    }
    if (case_statement_)
        case_statement_->Print(stream);
}

} // namespace ast
