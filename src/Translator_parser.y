%code requires{
  #include "../inc/Translator_ast_node.hpp"

  #include <cassert>
  extern const astP *proot;
  int zzlex(void);
  void zzerror(const char *);
}

%token IDENTIFIER CONSTANT
%token AND_OP OR_OP EQ_OP
%token T_INT T_VOID
%token T_IF T_ELSE T_WHILE T_RETURN

%union{
  astP* nodeptr;
  std::string* string;
}

%type <nodeptr> primary_expression postfix_expression argument_expression_list
%type <nodeptr> multiplicative_expression additive_expression relational_expression
%type <nodeptr> equality_expression logical_and_expression logical_or_expression
%type <nodeptr> assignment_expression expression declaration init_declarator_list
%type <nodeptr> init_declarator type_specifier direct_declarator parameter_list
%type <nodeptr> parameter_declaration block_item_list block_item
%type <nodeptr> statement compound_statement
%type <nodeptr> expression_statement selection_statement iteration_statement
%type <nodeptr> jump_statement translation_unit
%type <nodeptr> function_definition start_proot
%type <string> IDENTIFIER CONSTANT

%start start_proot
%%

primary_expression
	: IDENTIFIER
    { $$ = new id_p(*$1); }
	| CONSTANT
    { $$ = new id_p(*$1); }
	| '(' expression ')' { $$ = $2; }
	;

postfix_expression
	: primary_expression { $$ = $1; }
	| postfix_expression '(' ')'
    { $$ = new postfix_expression_p($1); }
	| postfix_expression '(' argument_expression_list ')'
    { $$ = new postfix_expression_p($1, $3); }
	;

argument_expression_list
	: assignment_expression
    { $$ = new argument_expression_list_p($1); }
	| argument_expression_list ',' assignment_expression
    { $$ = new argument_expression_list_p($1, $3); }
	;

multiplicative_expression
	: postfix_expression   { $$ = $1; }
	| multiplicative_expression '*' postfix_expression
    { $$ = new binary_expression_p("*",$1,$3); }
	;

additive_expression
	: multiplicative_expression    { $$ = $1; }
	| additive_expression '+' multiplicative_expression
    { $$ = new binary_expression_p("+",$1,$3); }
	| additive_expression '-' multiplicative_expression
    { $$ = new binary_expression_p("-",$1,$3); }
	;

relational_expression
	: additive_expression  { $$ = $1; }
	| relational_expression '<' additive_expression
    { $$ = new binary_expression_p("<",$1,$3); }
    | relational_expression '>' additive_expression
    { $$ = new binary_expression_p(">",$1,$3); }
	;

equality_expression
	: relational_expression  { $$ = $1; }
	| equality_expression EQ_OP relational_expression
    { $$ = new binary_expression_p("==",$1,$3); }
	;

logical_and_expression
	: equality_expression  { $$ = $1; }
	| logical_and_expression AND_OP equality_expression
    { $$ = new binary_expression_p("and",$1,$3); }
	;

logical_or_expression
	: logical_and_expression { $$ = $1; }
	| logical_or_expression OR_OP logical_and_expression
    { $$ = new binary_expression_p("or",$1,$3); }
	;

assignment_expression
	: logical_or_expression { $$ = $1; }
	| postfix_expression '=' assignment_expression
    { $$ = new assignment_expression_p($1, $3); }
	;

expression
	: assignment_expression
    { $$ = new expression_p($1); }
	| expression ',' assignment_expression
    { $$ = new expression_p($1, $3); }
	;

declaration
	: type_specifier init_declarator_list ';'
    { $$ = new declaration_p($2);}
	;

init_declarator_list
	: init_declarator
    { $$ = new init_declarator_list_p($1); }
	| init_declarator_list ',' init_declarator
    { $$ = new init_declarator_list_p($1, $3); }
	;

init_declarator
	: direct_declarator
    { $$ = new init_declarator_p($1); }
	| direct_declarator '=' assignment_expression
    { $$ = new init_declarator_p($1, $3); }
	;

type_specifier
	: T_VOID {;}
	| T_INT {;}
	;

direct_declarator
	: IDENTIFIER
    {  $$ = new direct_declarator_p( new id_p(*$1), false); }
	| IDENTIFIER '(' parameter_list ')'
    { $$ = new direct_declarator_p( new id_p(*$1), true, $3); }
	| IDENTIFIER '(' ')'
    { $$ = new direct_declarator_p( new id_p(*$1), true); }
	;

parameter_list
	: parameter_declaration
    { $$ = new parameter_list_p($1); }
	| parameter_list ',' parameter_declaration
    { $$ = new parameter_list_p($1, $3); }
	;

parameter_declaration
	: type_specifier direct_declarator
    { $$ = new parameter_declaration_p($2); }
	;

statement
	: compound_statement       { $$ = new statement_p($1); }
	| expression_statement     { $$ = new statement_p($1); }
	| selection_statement      { $$ = new statement_p($1); }
	| iteration_statement      { $$ = new statement_p($1); }
	| jump_statement           { $$ = new statement_p($1); }
	;

compound_statement
  : '{' '}'
  { $$ = new compound_statement_p(); }
  | '{'  block_item_list '}'
  { $$ = new compound_statement_p($2); }
  ;

block_item_list
	: block_item
    { $$ = new block_item_list_p($1); }
	| block_item_list block_item
    { $$ = new block_item_list_p($1, $2); }
	;

block_item
	: declaration
    { $$ = new block_item_p($1); }
	| statement
    { $$ = new block_item_p($1); }
	;

expression_statement
	: ';'             { $$ = new expression_statement_p(); }
	| expression ';'  { $$ = new expression_statement_p($1); }
	;

selection_statement
	: T_IF '(' expression ')' statement
    { $$ = new selection_statement_p($3, $5); }
	| T_IF '(' expression ')' statement T_ELSE statement
    { $$ = new selection_statement_p($3, $5, $7); }
	;

iteration_statement
	: T_WHILE '(' expression ')' statement
    { $$ = new iteration_statement_p($3, $5); }
	;

jump_statement
	: T_RETURN ';'
    { $$ = new jump_statement_p();}
	| T_RETURN expression ';'
    { $$ = new jump_statement_p($2); }
	;

translation_unit
	: function_definition
    { $$ = new translation_unit_p($1); }
	| declaration
    { $$ = new translation_unit_p($1); }
	| translation_unit function_definition
    { $$ = new translation_unit_p($1, $2); }
	| translation_unit declaration
    { $$ = new translation_unit_p($1, $2); }
	;


function_definition
	: type_specifier direct_declarator compound_statement
    { $$ = new function_p($2, $3); }
	;

start_proot
	: translation_unit	{ proot = $1; }
	;

%%


const astP *proot;

const astP *parsePAST()
{
  proot = NULL;
  zzparse();
  return proot;
}
