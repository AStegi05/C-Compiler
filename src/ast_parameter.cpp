#include "ast_context.hpp"
#include "ast_identifier.hpp"
#include "ast_parameter.hpp"

namespace ast {

void ParameterDeclaration::EmitRISC(std::ostream& stream, Context& context) const {

    std::string name = declarator_->getIdentifier();
    int pointerDepth = declarator_->isPointer();
    int offset = context.declareParameter(name, type_specifier_, pointerDepth);
    Context::ParamInfo info = context.getParamInfo(name);

    // we ain't doin mre than 8 so this works (even though ik it's bad)
    if (info.regIntIndex < 8 && info.regFloatIndex < 8) {
        declarator_->EmitRISC(stream, context);

        if (pointerDepth > 0) {
            stream << "sw a" << info.regIntIndex << ", " << offset << "(s0)" << std::endl;
        } else if (type_specifier_ == TypeSpecifier::CHAR) {
            stream << "sb a" << info.regIntIndex << ", " << offset << "(s0)" << std::endl;
        } else if (type_specifier_ == TypeSpecifier::FLOAT) {
            stream << "fsw fa" << info.regFloatIndex << ", " << offset << "(s0)" << std::endl;
        } else if (type_specifier_ == TypeSpecifier::DOUBLE) {
            stream << "fsd fa" << info.regFloatIndex << ", " << offset << "(s0)" << std::endl;
        } else {
            stream << "sw a" << info.regIntIndex << ", " << offset << "(s0)" << std::endl;
        }
    }
    else {
        throw std::runtime_error("Too many parameters");
    }
}

void ParameterDeclaration::Print(std::ostream& stream) const {
    stream << type_specifier_ << " ";
    declarator_->Print(stream);
}

} // namespace ast
