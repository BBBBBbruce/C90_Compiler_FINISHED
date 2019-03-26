%code requires{
  #include "../inc/MIPS_ast_node.hpp"

  #include <cassert>

  extern const astM *root;
  int yylex(void);
  void yyerror(const char *);
}

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME
%token TYPEDEF EXTERN
%token CHAR SHORT INT LONG  UNSIGNED FLOAT DOUBLE   VOID
%token STRUCT ENUM
%token CASE DEFAULT IF ELSE SWITCH WHILE  FOR  CONTINUE BREAK RETURN

%union{
  astM* nodeptr;
  std::string* string;
  char character;
}

%type <nodeptr> primary_expression postfix_expression argument_expression_list
%type <nodeptr> multiplicative_expression additive_expression relational_expression
%type <nodeptr> equality_expression and_expression logical_and_expression
%type <nodeptr> assignment_expression expression declaration init_declarator_list
%type <nodeptr> init_declarator type_specifier direct_declarator parameter_list
%type <nodeptr> parameter_declaration block_item_list block_item
%type <nodeptr> statement compound_statement logical_or_expression
%type <nodeptr> expression_statement selection_statement iteration_statement
%type <nodeptr> jump_statement translation_unit
%type <nodeptr> function_definition conditional_expression
%type <nodeptr> unary_expression unary_operator cast_expression
%type <nodeptr> shift_expression exclusive_or_expression inclusive_or_expression
%type <nodeptr> constant_expression declaration_specifiers
%type <nodeptr> storage_class_specifier struct_specifier struct_declaration_list
%type <nodeptr> struct_declaration specifier_qualifier_list struct_declarator_list
%type <nodeptr> struct_declarator enum_specifier enumerator_list enumerator
%type <nodeptr> declarator pointer type_name abstract_declarator
%type <nodeptr> direct_abstract_declarator initializer initializer_list
%type <nodeptr> labeled_statement external_declaration
%type <nodeptr> assignment_operator
%type <string> VOID CHAR INT FLOAT DOUBLE UNSIGNED TYPE_NAME IDENTIFIER CONSTANT STRING_LITERAL

%start start_root
%%

primary_expression
	: IDENTIFIER           { $$ = new primary_expression(*$1, 'i'); }
	| CONSTANT             { $$ = new primary_expression(*$1, 'c'); }
	| STRING_LITERAL       { $$ = new primary_expression(*$1, 's'); }
	| '(' expression ')'   { $$ = new primary_expression($2); }
	;

postfix_expression
	: primary_expression   { $$ = new postfix_expression($1, 'n'); }
	| postfix_expression '[' expression ']'
        { $$ = new postfix_expression($1, $3, '['); }
	| postfix_expression '(' ')'
        { $$ = new postfix_expression($1, '('); }
	| postfix_expression '(' argument_expression_list ')'
        { $$ = new postfix_expression($1, $3, '('); }
	| postfix_expression '.' IDENTIFIER
        {std::cerr<<"e: postfix_expression 4.\n";$$=NULL;}
	| postfix_expression PTR_OP IDENTIFIER
        {std::cerr<<"e: postfix_expression 5.\n";$$=NULL;}
	| postfix_expression INC_OP
        {std::cerr<<"e: postfix_expression 6.\n";$$=NULL;}
	| postfix_expression DEC_OP
        {std::cerr<<"e: postfix_expression 7.\n";$$=NULL;}
	;

argument_expression_list
	: assignment_expression    { $$ = new argument_expression_list($1); }
	| argument_expression_list ',' assignment_expression
        { $$ = new argument_expression_list($1, $3); }
	;

unary_expression
	: postfix_expression
        {  $$ = new unary_expression($1, 'p'); }
	| INC_OP unary_expression
        {  $$ = new unary_expression($2, 'i'); }
	| DEC_OP unary_expression
        {  $$ = new unary_expression($2, 'd'); }
	| unary_operator cast_expression
        {  $$ = new unary_expression($1, $2); }
	| SIZEOF unary_expression
        { $$ = new unary_expression($2, 's'); }
	| SIZEOF '(' type_name ')'
        { std::cerr << "e: unary_expression 5.\n"; $$ = NULL; }
	;

unary_operator
	: '&'  { $$ = new unary_operator('&'); }
	| '*'  { $$ = new unary_operator('*'); }
	| '+'  { $$ = new unary_operator('+'); }
	| '-'  { $$ = new unary_operator('-'); }
	| '~'  { $$ = new unary_operator('~'); }
	| '!'  { $$ = new unary_operator('!'); }
	;

cast_expression
	: unary_expression { $$ = new cast_expression($1); }
	| '(' type_name ')' cast_expression
        { std::cerr<<"e: no cast expression.\n"; $$ = NULL; }
	;

multiplicative_expression
	: cast_expression { $$ = new bin_expression($1);}
	| multiplicative_expression '*' cast_expression
    { $$ = new bin_expression($1, $3, '*'); }
	| multiplicative_expression '/' cast_expression
    { $$ = new bin_expression($1, $3, '/'); }
	| multiplicative_expression '%' cast_expression
    { $$ = new bin_expression($1, $3, '%'); }
	;

additive_expression
	: multiplicative_expression { $$ = new bin_expression($1);}
	| additive_expression '+' multiplicative_expression
        { $$ = new bin_expression($1, $3, '+'); }
	| additive_expression '-' multiplicative_expression
        { $$ = new bin_expression($1, $3, '-'); }
	;

shift_expression
	: additive_expression { $$ = new bin_expression($1);}
	| shift_expression LEFT_OP additive_expression
        { $$ = new bin_expression($1, $3, 'l'); }
	| shift_expression RIGHT_OP additive_expression
        { $$ = new bin_expression($1, $3, 'r'); }
	;

relational_expression
	: shift_expression { $$ = new bin_expression($1);}
	| relational_expression '<' shift_expression
        { $$ = new bin_expression($1, $3, '<'); }
	| relational_expression '>' shift_expression
        { $$ = new bin_expression($1, $3, '>'); }
	| relational_expression LE_OP shift_expression
        { $$ = new bin_expression($1, $3, 's'); }
	| relational_expression GE_OP shift_expression
        { $$ = new bin_expression($1, $3, 'g'); }
	;

equality_expression
	: relational_expression { $$ = new bin_expression($1);}
	| equality_expression EQ_OP relational_expression
        { $$ = new bin_expression($1, $3, 'e'); }
	| equality_expression NE_OP relational_expression
        { $$ = new bin_expression($1, $3, 'n'); }
	;

and_expression
	: equality_expression { $$ = new bin_expression($1);}
	| and_expression '&' equality_expression
        { $$ = new bin_expression($1, $3, '&'); }
	;

exclusive_or_expression
	: and_expression { $$ = new bin_expression($1);}
	| exclusive_or_expression '^' and_expression
        { $$ = new bin_expression($1, $3, '^'); }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = new bin_expression($1);}
	| inclusive_or_expression '|' exclusive_or_expression
        { $$ = new bin_expression($1, $3, '|'); }
	;

logical_and_expression
	: inclusive_or_expression { $$ = new bin_expression($1);}
	| logical_and_expression AND_OP inclusive_or_expression
        { $$ = new bin_expression($1, $3, 'a'); }
	;

logical_or_expression
	: logical_and_expression { $$ = new bin_expression($1);}
	| logical_or_expression OR_OP logical_and_expression
        { $$ = new bin_expression($1, $3, 'o'); }
	;

conditional_expression
	: logical_or_expression
    { $$ = new conditional_expression($1); }
	| logical_or_expression '?' expression ':' conditional_expression
    { $$ = new conditional_expression($1,$3,$5); }
	;

assignment_expression
	: conditional_expression
        { $$ = new assignment_expression($1); }
	| unary_expression assignment_operator assignment_expression
        { $$ = new assignment_expression($1, $2, $3); }
	;

assignment_operator
	: '='          { $$ = new assignment_operator('='); }
	| MUL_ASSIGN   { $$ = new assignment_operator('*'); }
	| DIV_ASSIGN   { $$ = new assignment_operator('/'); }
	| MOD_ASSIGN   { $$ = new assignment_operator('%'); }
	| ADD_ASSIGN   { $$ = new assignment_operator('+'); }
	| SUB_ASSIGN   { $$ = new assignment_operator('-'); }
	| LEFT_ASSIGN  { $$ = new assignment_operator('l'); }
	| RIGHT_ASSIGN { $$ = new assignment_operator('r'); }
	| AND_ASSIGN   { $$ = new assignment_operator('&'); }
	| XOR_ASSIGN   { $$ = new assignment_operator('^'); }
	| OR_ASSIGN    { $$ = new assignment_operator('|'); }
	;

expression
	: assignment_expression
    { $$ = new expression($1); }
	| expression ',' assignment_expression
    { $$ = new expression($1, $3); }

	;

constant_expression
	: conditional_expression   { $$ = new constant_expression($1); }
	;

declaration
	: declaration_specifiers ';'
    { /* THINK ABOUT IT */  $$ = NULL; }
	| declaration_specifiers init_declarator_list ';'
        { $$ = new declaration($1, $2); }
	;

declaration_specifiers
	: storage_class_specifier
        { $$ = new declaration_specifiers($1); }
	| storage_class_specifier declaration_specifiers
        { $$ = new declaration_specifiers($1,$2); }
	| type_specifier
        { $$ = new declaration_specifiers($1); }
	| type_specifier declaration_specifiers
        { $$ = new declaration_specifiers($1,$2); }
	;

init_declarator_list
	: init_declarator
        { $$ = new init_declarator_list($1); }
	| init_declarator_list ',' init_declarator
        {std::cerr<<"e: parser: 1 var - init_dec_list\n"; $$ = NULL;}
	;

init_declarator
	: declarator
        { $$ = new init_declarator($1); }
	| declarator '=' initializer
        { $$ = new init_declarator($1, $3); }
	;

storage_class_specifier
	: TYPEDEF  { $$ = NULL; }
	| EXTERN   { $$ = NULL; }
	;

type_specifier
	: VOID     { $$ = new type_specifier("void");     }
	| CHAR     { $$ = new type_specifier("char");     }
	| INT      { $$ = new type_specifier("int");      }
	| FLOAT    { $$ = new type_specifier("float");    }
	| DOUBLE   { $$ = new type_specifier("double");   }
	| UNSIGNED { $$ = new type_specifier("unsigned"); }
	| struct_specifier { $$ = new type_specifier($1); }
	| enum_specifier   { $$ = new type_specifier($1); }
	| TYPE_NAME        { $$ = new type_specifier(*$1);}
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}'
        { $$ = NULL; }
	| STRUCT '{' struct_declaration_list '}'
        { $$ = NULL; }
	| STRUCT IDENTIFIER
        { $$ = NULL; }
	;

struct_declaration_list
	: struct_declaration   { $$ = NULL; }
	| struct_declaration_list struct_declaration
        { $$ = NULL; }
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
        { $$ = NULL; }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list  { $$ = NULL; }
	| type_specifier                           { $$ = NULL; }
	;

struct_declarator_list
	: struct_declarator    { $$ = NULL; }
	| struct_declarator_list ',' struct_declarator
        { $$ = NULL; }
	;

struct_declarator
	: declarator                           { $$ = NULL; }
	| ':' constant_expression              { $$ = NULL; }
	| declarator ':' constant_expression   { $$ = NULL; }
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
        { $$ = NULL; }
	| ENUM IDENTIFIER '{' enumerator_list '}'
        { $$ = NULL; }
	| ENUM IDENTIFIER
        { $$ = NULL; }
	;

enumerator_list
	: enumerator                       { $$ = NULL; }
	| enumerator_list ',' enumerator   { $$ = NULL; }
	;

enumerator
	: IDENTIFIER
        { $$ = NULL; }
	| IDENTIFIER '=' constant_expression
        { $$ = NULL; }
	;


declarator
	: pointer direct_declarator
        { $$ = new declarator($2, $2); }
	| direct_declarator
        { $$ = new declarator($1); }
	;

direct_declarator
	: IDENTIFIER
        { $$ = new direct_declarator(*$1, 'i'); }
	| '(' declarator ')'
        { $$ = new direct_declarator($2, 'p'); }
	| direct_declarator '[' constant_expression ']'
        { $$ = new direct_declarator($1, $3, 'a'); }
	| direct_declarator '(' parameter_list ')'
        { $$ = new direct_declarator($1, $3, 'f'); }
	| direct_declarator '(' ')'
        { $$ = new direct_declarator($1, 'f'); }
	;

pointer
	: '*'
        {  $$ = NULL; }
	| '*' pointer
        {  $$ = NULL; }
	;

parameter_list
	: parameter_declaration
        { $$ = new parameter_list($1); }
	| parameter_list ',' parameter_declaration
        { $$ = new parameter_list($1, $3); }
	;

parameter_declaration
	: declaration_specifiers declarator
    { $$ = new parameter_declaration($1, $2); }
	| declaration_specifiers abstract_declarator
    { /* ignore */  $$ = NULL; }
	| declaration_specifiers
    { /* ignore */  $$ = NULL; }
	;

type_name
	: specifier_qualifier_list                     { $$ = NULL; }
	| specifier_qualifier_list abstract_declarator { $$ = NULL; }
	;

abstract_declarator
	: pointer                              { $$ = NULL; }
	| direct_abstract_declarator           { $$ = NULL; }
	| pointer direct_abstract_declarator   { $$ = NULL; }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
        { $$ = NULL; }
	| '[' ']'
        { $$ = NULL; }
	| '[' constant_expression ']'
        { $$ = NULL; }
	| direct_abstract_declarator '[' ']'
        { $$ = NULL; }
	| direct_abstract_declarator '[' constant_expression ']'
        { $$ = NULL; }
	| '(' ')'
        { $$ = NULL; }
	| '(' parameter_list ')'
        { $$ = NULL; }
	| direct_abstract_declarator '(' ')'
        { $$ = NULL; }
	| direct_abstract_declarator '(' parameter_list ')'
        { $$ = NULL; }
	;

initializer
	: assignment_expression
        { $$ = new initializer($1); }
	| '{' initializer_list '}'
        { $$ = NULL; }
	| '{' initializer_list ',' '}'
        { $$ = NULL; }
	;

initializer_list
	: initializer                      { $$ = NULL; }
	| initializer_list ',' initializer { $$ = NULL; }
	;

statement
	: labeled_statement    { $$ = NULL;  }
	| compound_statement   { $$ = new statement($1); }
	| expression_statement { $$ = new statement($1); }
	| selection_statement  { $$ = new statement($1); }
	| iteration_statement  { $$ = new statement($1); }
	| jump_statement       { $$ = new statement($1);  }
	;

labeled_statement
	: IDENTIFIER ':' statement
        { $$ = NULL; }
	| CASE constant_expression ':' statement
        { $$ = NULL; }
	| DEFAULT ':' statement
        { $$ = NULL; }
	;


block_item_list
	: block_item
    { $$ = new block_item_list($1); }
	| block_item_list block_item
    { $$ = new block_item_list($1, $2); }
	;

block_item
	: declaration
    { $$ = new block_item($1); }
	| statement
    { $$ = new block_item($1); }
	;


compound_statement
	: '{' '}'
        { $$ = new compound_statement(); }
	| '{' block_item_list '}'
        { $$ = new compound_statement($2); }
	;

expression_statement
	: ';'              { $$ = new expression_statement(); }
	| expression ';'   { $$ = new expression_statement($1); }
	;

selection_statement
	: IF '(' expression ')' statement
        { $$ = new selection_statement($3, $5, 'i'); }
	| IF '(' expression ')' statement ELSE statement
        { $$ = new selection_statement($3, $5, $7,'e'); }
	| SWITCH '(' expression ')' statement
        { $$ = new selection_statement($3, $5, 's'); }
	;

iteration_statement
	: WHILE '(' expression ')' statement
        { $$ = new iteration_statement($3, $5); }
    | FOR '(' expression_statement expression_statement ')' statement
        { $$ = new iteration_statement($3, $4, $6); }
    | FOR '(' declaration expression_statement ')' statement
        { $$ = new iteration_statement($3, $4, $6); }
	| FOR '(' expression_statement expression_statement expression ')' statement
        { $$ = new iteration_statement($3, $4, $7, $5); }
    | FOR '(' declaration expression_statement expression ')' statement
        { $$ = new iteration_statement($3, $4, $7, $5); }
	;

jump_statement
	: CONTINUE ';'          { $$ = new jump_statement('c'); }
	| BREAK ';'             { $$ = new jump_statement('b'); }
	| RETURN ';'            { $$ = new jump_statement('r'); }
	| RETURN expression ';' { $$ = new jump_statement($2,'r'); }
	;

function_definition
    : declaration_specifiers declarator compound_statement
        { $$ = new function_definition($1, $2, $3); }
    | declaration_specifiers declarator ';'
        { $$ = NULL; }
    ;

external_declaration
	: function_definition { $$ = new external_declaration($1); }
	| declaration 		  { $$ = new external_declaration($1); }
	;

translation_unit
	: external_declaration
    { $$ = new translation_unit($1); }
	| translation_unit external_declaration
    { $$ = new translation_unit($1, $2); }
	;

start_root
	: translation_unit	{ root = $1; }
	;

%%
const astM *root;

const astM *parseAST()
{
  root = NULL;
  yyparse();
  return root;
}


/*
usless rules

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

direct_declarator
    | direct_declarator '[' ']'
        { std::cerr<<"e: direct_declarator not implemented 3.\n"; }
    ;

*/
