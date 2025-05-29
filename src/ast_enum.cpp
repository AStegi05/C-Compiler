// ast_enum.cpp
#include "ast_enum.hpp"
#include "ast_context.hpp"
#include "ast_init_declarator.hpp"
#include "ast_identifier.hpp"

namespace ast {

    void Enumerator::EmitRISC(std::ostream& stream, Context& context) const {
        (void)stream;
        (void)context;
    }

    void Enumerator::Print(std::ostream& stream) const {
        stream << "ENUM deb" << std::endl;
        // stream << identifier_;
        // if (value_) {

        //     stream << " = ";
        //     value_->Print(stream);
        }


    void EnumSpecifier::EmitRISC(std::ostream& stream, Context& context) const {

        if (!members_) {
            return;  // Nothing to do for forward declarations
        }

        std::vector<std::string> enums = members_->getEnumNames();
        std::vector<std::variant<int, float, double>> consts = members_->getConstList();

        int value = 0;
        for (size_t i = 0; i < enums.size(); ++i) {
            if (i < consts.size() && std::holds_alternative<int>(consts[i])) {
                value = std::get<int>(consts[i]);
            }

            if (enums[i] != "") {
                stream << "    li a5, " << value << std::endl;
                int offset = context.declareVariable(enums[i], TypeSpecifier::INT);
                stream << "    sw a5, " << offset << "(s0)" << std::endl;
            }

            value++;
        }
    }

    void EnumSpecifier::Print(std::ostream& stream) const {
        stream << "ENUM deb" << std::endl;
        // stream << "enum ";
        // if (identifier_) {
        //     identifier_->Print(stream);
        // }
        // stream << " {";
        // if (members_) {
        //     members_->Print(stream);
        // }
        // stream << "}";
    }

} // namespace ast
