#include "ast_other.hpp"
#include "ast_node.hpp"
#include "ast_context.hpp"
#include <iostream>
#include <stdexcept>

namespace ast {
    void SizeoffExprFunction::EmitRISC(std::ostream& stream, Context& context) const {


        // Find the array variable
        Variable* var = context.findVariable(name);
        std::cerr << "DEBUG: var is "<<var <<" and Expr is " << Expr_ <<" and name is "<< name << std::endl;
        if (var) {
            int a = var->isArray();
            if(Expr_->isFloatExpression(context)){
                stream << "li a5, "<< 4*a << std::endl;
            }
            else if(Expr_->isDoubleExpression(context)){
                stream << "li a5, "<< 8*a << std::endl;
            }
            else if(Expr_->isCharExpression(context)){
                stream << "li a5, "<< 1*a<< std::endl;
            }
            else{
                stream << "li a5, "<< 4 * a<< std::endl;
            }
        } else {
            if(Expr_->isFloatExpression(context)){
                stream << "li a5, 4"<< std::endl;
            }
            else if(Expr_->isDoubleExpression(context)){
                stream << "li a5, 8"<< std::endl;
            }
            else if(Expr_->isCharExpression(context)){
                stream << "li a5, 1"<< std::endl;
            }
            else{
                stream << "li a5, 4"<< std::endl;
            }
        }

    }

    void SizeoffExprFunction::Print(std::ostream& stream) const {
        stream << "sizeoff(";
        if (Expr_) {
            Expr_->Print(stream);
        }
        stream << ")";
    }

    void SizeoffTypeFunction::EmitRISC(std::ostream& stream, Context&) const {
        std::string name = getIdentifier();
        stream << "  # name " << name <<  std::endl;
        if(name == "float"){
            stream << "li a5, 4"<< std::endl;
        }
        else if(name == "double"){
            stream << "li a5, 8"<< std::endl;
        }
        else if(name == "char"){
            stream << "li a5, 1"<< std::endl;
        }
        else{
            stream << "li a5, 4"<< std::endl;
        }
    }

    void SizeoffTypeFunction::Print(std::ostream& stream) const {
        stream << "sizeoff(";
        if (Type_) {
            Type_->Print(stream);
        }
        stream << ")";
    }

} // namespace ast
