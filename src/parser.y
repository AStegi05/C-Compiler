// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

// TODO: you can either continue adding to this grammar file or
// rename parser_full.y to parser.y once you're happy with
// how this example works.

%define parse.error verbose
%debug

%code requires{
    #include "ast.hpp"

    using namespace ast;

    extern Node* g_root;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
    int yylex_destroy(void);
}

%code {
    // Global variables for the parser
    TypeSpecifier current_type_context = TypeSpecifier::DOUBLE; // Default to double per C90
}

%union{
  Node*				node;
  NodeList*			node_list;
  int          		number_int;
  double       		number_float;
  std::string*		string;
  TypeSpecifier 	type_specifier;
  yytokentype  		token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL CHAR_CONSTANT
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> external_declaration function_definition function_declaration primary_expression postfix_expression argument_expression_list
%type <node> unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression
%type <node> equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <node> conditional_expression assignment_expression expression declaration init_declarator_list constant_expression
%type <node> init_declarator struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier  enumerator declarator direct_declarator pointer variable_declaration parameter_declaration
%type <node> type_name direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%type <node_list> statement_list parameter_list translation_unit enumerator_list labeled_statement_list

%type <number_int> INT_CONSTANT
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER STRING_LITERAL
%type <type_specifier> type_specifier declaration_specifiers


%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }

translation_unit
	: external_declaration { $$ = new NodeList(NodePtr($1)); }
	| translation_unit external_declaration { $1->PushBack(NodePtr($2)); $$=$1; }
	;

external_declaration
	: function_definition { $$ = $1; }
	| function_declaration { $$ = $1; }
	| variable_declaration { $$ = $1; }
	;

function_definition
	: declaration_specifiers declarator compound_statement {
		$$ = new FunctionDefinition($1, NodePtr($2), NodePtr($3));
	}
	;

function_declaration
    : declaration_specifiers direct_declarator '(' ')' ';' {
        $$ = new FunctionDeclaration($1, NodePtr($2), nullptr);
    }
    | declaration_specifiers direct_declarator '(' parameter_list ')' ';' {
        $$ = new FunctionDeclaration($1, NodePtr($2), NodePtr($4));
    }
    ;

variable_declaration
    : declaration_specifiers init_declarator_list ';' {
        $$ = new VariableDeclare($1, NodePtr($2));
    }
    ;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';' { $$ = new VariableDeclare($1, NodePtr($2)); }
	;

declaration_specifiers
	: type_specifier { $$ = $1; current_type_context = $1; }
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator { $$ = new InitDeclarator(NodePtr($1), nullptr); } // The goal here is to just declare the variable eg. 'int a;'
	| declarator '=' initializer { $$ = new InitDeclarator(NodePtr($1), NodePtr($3)); } // The goal here is to declare and initialize the variable eg. 'int a = 5;'
	;

type_specifier
    : INT {$$ = TypeSpecifier::INT; current_type_context = TypeSpecifier::INT;}
    | FLOAT {$$ = TypeSpecifier::FLOAT; current_type_context = TypeSpecifier::FLOAT;}
    | DOUBLE {$$ = TypeSpecifier::DOUBLE; current_type_context = TypeSpecifier::DOUBLE;}
    | CHAR {$$ = TypeSpecifier::CHAR; current_type_context = TypeSpecifier::CHAR;}
	| VOID {$$ = TypeSpecifier::VOID; current_type_context = TypeSpecifier::VOID;}
	| UNSIGNED {$$ = TypeSpecifier::UNSIGNED; current_type_context = TypeSpecifier::UNSIGNED;}
	| UNSIGNED INT {$$ = TypeSpecifier::UNSIGNED; current_type_context = TypeSpecifier::UNSIGNED;}
    | enum_specifier {$$ = TypeSpecifier::ENUM; current_type_context = TypeSpecifier::INT;}
	| TYPE_NAME
	;

enum_specifier
    : ENUM '{' enumerator_list '}' ';'{
        std::cerr << "DEBUG: Creating EnumSpecifier node in enum_specifier rule1" << std::endl;
        $$ = new EnumSpecifier(nullptr, NodePtr($3));
    }
    | ENUM IDENTIFIER '{' enumerator_list '}'';' {
        std::cerr << "DEBUG: Creating EnumSpecifier node in enum_specifier rule2" << std::endl;
        $$ = new EnumSpecifier(NodePtr(new Identifier(std::move(*$2))), NodePtr($4));
        delete $2;
    }
    | ENUM IDENTIFIER ';'{
        std::cerr << "DEBUG: Creating EnumSpecifier node in enum_specifier rule3" << std::endl;
        $$ = new EnumSpecifier(NodePtr(new Identifier(std::move(*$2))), nullptr);
        delete $2;
    }
    ;

enumerator_list
	: enumerator {
        std::cerr << "DEBUG: Creating Enumerator node in enumerator_list rule" << std::endl;
        $$ = new NodeList(NodePtr($1)); }
	| enumerator_list ',' enumerator {
        std::cerr << "DEBUG: Creating Enumerator node in enumerator_list rule" << std::endl;
        $1->PushBack(NodePtr($3)); $$ = $1; }
	;

enumerator
    : IDENTIFIER {
        std::cerr << "DEBUG: Creating Enumerator node in enumerator rule" << std::endl;
        $$ = new Enumerator(NodePtr(new Identifier(std::move(*$1))), nullptr);
        delete $1;
    }
    | IDENTIFIER '=' constant_expression {
        std::cerr << "DEBUG: Creating Enumerator node in enumerator rule" << std::endl;
        $$ = new Enumerator(NodePtr(new Identifier(std::move(*$1))), NodePtr($3));
        delete $1;
	}
    ;

pointer
    : '*' { $$ = new IntConstant(1); } // Single pointer level
    | '*' pointer {
        // Nested pointer levels (e.g., **)
        int level = static_cast<IntConstant*>($2)->getConstantValue() + 1;
        $$ = new IntConstant(level);
    }
    ;

declarator
	: pointer direct_declarator { $$ = new PointerDeclarator(NodePtr($2), 1); }
	| direct_declarator { $$ = $1; }
	;

constant_expression
	: conditional_expression
	;

direct_declarator
	: IDENTIFIER {
		$$ = new Identifier(std::move(*$1));
		delete $1;
	}
	| '(' declarator ')'
	| direct_declarator '[' INT_CONSTANT ']' { $$ = new DeclareArray(NodePtr($1), $3); }
	| direct_declarator '[' ']' { throw std::runtime_error("Array declarations without size not supported in C90"); }
	| direct_declarator '(' parameter_list ')' { $$ = new DirectDeclarator(NodePtr($1), NodePtr($3)); }
//	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')' { $$ = new DirectDeclarator(NodePtr($1), nullptr); }
	;

parameter_list
	: parameter_declaration { $$ = new NodeList(NodePtr($1)); }
	| parameter_list ',' parameter_declaration { $1->PushBack(NodePtr($3)); $$=$1; }
	;

parameter_declaration
	: declaration_specifiers declarator {
        // Pass the type specifier and declarator to the ParameterDeclaration constructor
        // The ParameterDeclaration class will extract the pointer level from the declarator
        $$ = new ParameterDeclaration($1, NodePtr($2));
    }
	| declaration_specifiers { $$ = nullptr; }
	;

initializer
	: assignment_expression
	| '{' initializer_list '}' { $$ = $2; }
	| '{' initializer_list ',' '}' { $$ = $2; }
	;

initializer_list
    : initializer {
        $$ = new NodeList(NodePtr($1));
    }
    | initializer_list ',' initializer {
        // Cast to NodeList* before calling PushBack
        static_cast<NodeList*>($1)->PushBack(NodePtr($3));
        $$ = $1;
    }
    ;
type_name
	: type_specifier
	;



statement
	: declaration { $$ = $1; } // variable declaration
	| jump_statement { $$ = $1; } // return statement
	| expression_statement { $$ = $1; }
    | compound_statement { $$ = $1; }
	| selection_statement { $$ = $1; }
	| iteration_statement { $$ = $1; }
	;

labeled_statement
	: CASE constant_expression ':' statement_list { $$ = new Case(NodePtr($2),NodePtr( $4)); }
	| DEFAULT ':' statement_list { $$ = new Case(NodePtr($3), nullptr); }
	;
labeled_statement_list
    : labeled_statement { $$ = new NodeList(NodePtr($1)); }
    | labeled_statement_list labeled_statement {
          static_cast<NodeList*>($1)->PushBack(NodePtr($2));
          $$ = $1;
      }
    ;

compound_statement
    : '{' '}' { $$ = new CompoundStatement(nullptr); }
    | '{' statement_list '}' { $$ = new CompoundStatement(NodePtr($2)); }
    ;

statement_list
	: statement { $$ = new NodeList(NodePtr($1)); }
	| statement_list statement { $1->PushBack(NodePtr($2)); $$=$1; }
	;

jump_statement
	: RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement(NodePtr($2));
	}
	| CONTINUE ';'{$$ = new ContinueStatement();}
	| BREAK ';'{$$ = new BreakStatement();}
	;

primary_expression
	: IDENTIFIER {
		$$ = new VariableCall(NodePtr(new Identifier(std::move(*$1))));
		delete $1;
	}
	| INT_CONSTANT { $$ = new IntConstant($1, current_type_context); }
    | FLOAT_CONSTANT { $$ = new FloatConstant($1, current_type_context); }
	| STRING_LITERAL { $$ = new StringLiteral(std::move(*$1)); delete $1; }
	| '(' expression ')' { $$ = $2; }
	;

argument_expression_list
	: assignment_expression  {$$ = new NodeList(NodePtr($1));}
	| argument_expression_list ',' assignment_expression {static_cast<NodeList*>($1)->PushBack(NodePtr($3)); $$=$1; }
	;

postfix_expression
	: primary_expression { $$ = $1; }
	| IDENTIFIER '[' expression ']' {
        std::cerr << "DEBUG: Creating AccessArray node in postfix_expression rule" << std::endl;
        $$ = new AccessArray(NodePtr(new Identifier(std::move(*$1))), NodePtr($3));
        delete $1;
    }
	| IDENTIFIER '(' ')' {
		std::cerr << "DEBUG: FunctionCall detected with name = " << *$1 << std::endl;
		$$ = new FunctionCall(*$1, nullptr);
		delete $1;
	}
    | IDENTIFIER '(' argument_expression_list ')'{
        $$ = new FunctionCall(*$1, NodePtr($3));  // FunctionCall now stores argument list
        delete $1;
    }
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP { $$ = new Inc_Opp(NodePtr($1)); }
	| postfix_expression DEC_OP { $$ = new Dec_Opp(NodePtr($1)); }
	;

unary_expression
	: postfix_expression { $$ = $1; }
	| INC_OP unary_expression { $$ = new Inc_Opp(NodePtr($2)); }
	| DEC_OP unary_expression { $$ = new Dec_Opp(NodePtr($2)); }
	| '&' cast_expression { $$ = new AddressOf(NodePtr($2)); }
	| '*' cast_expression { $$ = new Dereference(NodePtr($2)); }
	| '-' cast_expression { $$ = new NegateOperator(NodePtr($2)); }
	| '+' cast_expression { $$ = new PosOperator(NodePtr($2)); }
	| '!'cast_expression {$$ = new LogicalNotOperator(NodePtr($2));}
	| '~'cast_expression {$$ = new BinaryNotOperator(NodePtr($2));}
	| SIZEOF unary_expression { $$ = new SizeoffExprFunction(NodePtr($2));}
	| SIZEOF '(' type_name ')' {$$ = new SizeoffTypeFunction(NodePtr($3));}
	;

unary_operator
	:'&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression { $$ = $1; }
	/* | '(' type_name ')' cast_expression */
	;


multiplicative_expression
	: cast_expression { $$ = $1; }
	| multiplicative_expression '*' cast_expression {$$ = new MultExpression(NodePtr($1), NodePtr($3)); }
	| multiplicative_expression '/' cast_expression {$$ = new DivExpression(NodePtr($1), NodePtr($3)); }
	| multiplicative_expression '%' cast_expression {$$ = new ModExpression(NodePtr($1), NodePtr($3)); }
	;

additive_expression
	: multiplicative_expression { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$ = new AddExpression(NodePtr($1), NodePtr($3)); }
	| additive_expression '-' multiplicative_expression { $$ = new SubExpression(NodePtr($1), NodePtr($3)); }
	;


shift_expression
	: additive_expression { $$ = $1; }
	| shift_expression LEFT_OP additive_expression { $$ = new LeftShiftOperator(NodePtr($1), NodePtr($3)); }
	| shift_expression RIGHT_OP additive_expression { $$ = new RightShiftOperator(NodePtr($1), NodePtr($3)); }
	;

relational_expression
	: shift_expression { $$ = $1; }
	| relational_expression '<' shift_expression { $$ = new LessThanOperator(NodePtr($1), NodePtr($3)); }
	| relational_expression '>' shift_expression { $$ = new BiggerThanOperator(NodePtr($1), NodePtr($3)); }
	| relational_expression LE_OP shift_expression { $$ = new LessThanEqOperator(NodePtr($1), NodePtr($3)); }
	| relational_expression GE_OP shift_expression { $$ = new BiggerThanEqOperator(NodePtr($1), NodePtr($3)); }
	;

equality_expression
	: relational_expression { $$ = $1; }
	| equality_expression EQ_OP relational_expression {$$ = new EqOperator(NodePtr($1), NodePtr($3));}
	| equality_expression NE_OP relational_expression {$$ = new NotEqOperator(NodePtr($1), NodePtr($3));}
	;

and_expression
	: equality_expression { $$ = $1; }
	| and_expression '&' equality_expression {$$ = new AndOperator(NodePtr($1), NodePtr($3));}
	;

exclusive_or_expression
	: and_expression { $$ = $1; }
	| exclusive_or_expression '^' and_expression {$$ = new XorOperator(NodePtr($1), NodePtr($3));}
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression {$$ = new OrOperator(NodePtr($1), NodePtr($3));}
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression {$$ = new LogicalAndOperator(NodePtr($1), NodePtr($3));}
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression {$$ = new LogicalOrOperator(NodePtr($1), NodePtr($3));}
	;

conditional_expression
	: logical_or_expression { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression {$$ = new TenaryOperator(NodePtr($1), NodePtr($3), NodePtr($5));}
	;

assignment_expression
    : conditional_expression { $$ = $1; }
    | IDENTIFIER '=' assignment_expression {
          $$ = new VariableAssign(std::move(*$1), NodePtr($3));
      }
    | unary_expression MUL_ASSIGN assignment_expression {$$ = new MultiplyAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression DIV_ASSIGN assignment_expression {$$ = new DivideAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression MOD_ASSIGN assignment_expression {$$ = new ModuloAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression ADD_ASSIGN assignment_expression {$$ = new PlusAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression SUB_ASSIGN assignment_expression {$$ = new MinusAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression LEFT_ASSIGN assignment_expression {$$ = new LeftShiftAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression RIGHT_ASSIGN assignment_expression {$$ = new RightShiftAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression AND_ASSIGN assignment_expression {$$ = new BitwiseAndAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression XOR_ASSIGN assignment_expression {$$ = new BitwiseXorAssignOperator(NodePtr($1), NodePtr($3)); }
    | unary_expression OR_ASSIGN assignment_expression {$$ = new BitwiseOrAssignOperator(NodePtr($1), NodePtr($3)); }
    | IDENTIFIER '[' expression ']' '=' assignment_expression {
          std::cerr << "DEBUG: Creating ArrayAssign for " << *$1 << std::endl;
          $$ = new ArrayAssign(std::move(*$1), NodePtr($3), NodePtr($6));
          delete $1;
      }
    | '*' unary_expression '=' assignment_expression {
          $$ = new AssignPointer(NodePtr($2), NodePtr($4));
      }
    ;

expression_statement
	: expression ';' { $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement { $$ = new IfStatement(NodePtr($3), NodePtr($5), nullptr); }
	| IF '(' expression ')' statement ELSE statement { $$ = new IfStatement(NodePtr($3), NodePtr($5), NodePtr($7)); }
    | SWITCH '(' expression ')' '{' labeled_statement_list '}'
          { $$ = new SwitchStatement(NodePtr($3), std::move(*$6)); }
	;

iteration_statement
	: WHILE '(' expression ')' statement { $$ = new WhileStatement(NodePtr($3), NodePtr($5)); }
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement { $$ = new ForStatement(NodePtr($3), NodePtr($4), nullptr, NodePtr($6)); }
	| FOR '(' expression_statement expression_statement expression ')' statement { $$ = new ForStatement(NodePtr($3), NodePtr($4), NodePtr($5), NodePtr($7)); }
	;


expression
	: assignment_expression
	;

%%

Node* g_root;

NodePtr ParseAST(std::string file_name)
{
  std::cerr << "Opening file: " << file_name << std::endl;
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == NULL){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  std::cerr << "Starting parse..." << std::endl;
  yyparse();
  std::cerr << "Parse complete!" << std::endl;
  fclose(yyin);
  yylex_destroy();
  return NodePtr(g_root);
}
