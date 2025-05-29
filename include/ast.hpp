#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ast_direct_declarator.hpp"
#include "ast_function_definition.hpp"
#include "ast_function_declaration.hpp"
#include "ast_function_call.hpp"
#include "ast_identifier.hpp"
#include "ast_jump_statement.hpp"
#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_constant.hpp"
#include "ast_context.hpp"
#include "ast_arithmetic.hpp"
#include "ast_fp.hpp"
#include "ast_variable.hpp"
#include "ast_init_declarator.hpp"
#include "ast_parameter.hpp"
#include "ast_logical_opp.hpp"
#include "compound_statement.hpp"
#include "if_statement.hpp"
#include "switch_statement.hpp"
#include "iteration_statement.hpp"
#include "ast_array.hpp"
#include "ast_enum.hpp"
#include "ast_pointer.hpp"
#include "ast_string.hpp"
#include "ast_other.hpp"

ast::NodePtr ParseAST(std::string file_name);
