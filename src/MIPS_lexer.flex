%option noyywrap

%{
extern "C" int fileno(FILE *stream);
#include "MIPS_parser.tab.hpp"

int check_type();

%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%%


"break"			{ yylval.string = new std::string(yytext); return(BREAK); 	}
"case"			{ yylval.string = new std::string(yytext); return(CASE); 	}
"char"			{ yylval.string = new std::string(yytext); return(CHAR); 	}
"continue"	{ yylval.string = new std::string(yytext); return(CONTINUE);}
"default"		{ yylval.string = new std::string(yytext); return(DEFAULT); }
"double"		{ yylval.string = new std::string(yytext); return(DOUBLE); 	}
"else"			{ yylval.string = new std::string(yytext); return(ELSE); 	}
"enum"			{ yylval.string = new std::string(yytext); return(ENUM); 	}
"extern"		{ yylval.string = new std::string(yytext); return(EXTERN); 	}//?
"float"			{ yylval.string = new std::string(yytext); return(FLOAT); 	}
"for"				{ yylval.string = new std::string(yytext); return(FOR); 	}
"if"				{ yylval.string = new std::string(yytext); return(IF); 		}
"int"				{ yylval.string = new std::string(yytext); return(INT); 	}
"return"		{ yylval.string = new std::string(yytext); return(RETURN); 	}
"sizeof"		{ yylval.string = new std::string(yytext); return(SIZEOF); 	}
"struct"		{ yylval.string = new std::string(yytext); return(STRUCT); 	}
"switch"		{ yylval.string = new std::string(yytext); return(SWITCH);	}
"typedef"		{ yylval.string = new std::string(yytext); return(TYPEDEF); }
"unsigned"	{ yylval.string = new std::string(yytext); return(UNSIGNED);}
"void"			{ yylval.string = new std::string(yytext); return(VOID); 	}
"while"			{ yylval.string = new std::string(yytext); return(WHILE); 	}

{L}({L}|{D})*		{ yylval.string = new std::string(yytext); return(check_type()); }

0[xX]{H}+{IS}?	  { yylval.string = new std::string(yytext); return(CONSTANT); }
0{D}+{IS}?		  { yylval.string = new std::string(yytext); return(CONSTANT); }
{D}+{IS}?		  { yylval.string = new std::string(yytext); return(CONSTANT); }
L?'(\\.|[^\\'])+' { yylval.string = new std::string(yytext); return(CONSTANT); }

{D}+{E}{FS}?			{ return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ return(STRING_LITERAL); }

">>="			{ return(RIGHT_ASSIGN); }
"<<="			{ return(LEFT_ASSIGN); }
"+="			{ return(ADD_ASSIGN); }
"-="			{ return(SUB_ASSIGN); }
"*="			{ return(MUL_ASSIGN); }
"/="			{ return(DIV_ASSIGN); }
"%="			{ return(MOD_ASSIGN); }
"&="			{ return(AND_ASSIGN); }
"^="			{ return(XOR_ASSIGN); }
"|="			{ return(OR_ASSIGN); }
">>"			{ return(RIGHT_OP); }
"<<"			{ return(LEFT_OP); }
"++"			{ return(INC_OP); }
"--"			{ return(DEC_OP); }
"->"			{ return(PTR_OP); }
"&&"			{ return(AND_OP); }
"||"			{ return(OR_OP); }
"<="			{ return(LE_OP); }
">="			{ return(GE_OP); }
"=="			{ return(EQ_OP); }
"!="			{ return(NE_OP); }
";"				{ return(';'); }
("{"|"<%")		{ return('{'); }
("}"|"%>")		{ return('}'); }
","				{ return(','); }
":"				{ return(':'); }
"="				{ return('='); }
"("				{ return('('); }
")"				{ return(')'); }
("["|"<:")		{ return('['); }
("]"|":>")		{ return(']'); }
"."				{ return('.'); }
"&"				{ return('&'); }
"!"				{ return('!'); }
"~"				{ return('~'); }
"-"				{ return('-'); }
"+"				{ return('+'); }
"*"				{ return('*'); }
"/"				{ return('/'); }
"%"				{ return('%'); }
"<"				{ return('<'); }
">"				{ return('>'); }
"^"				{ return('^'); }
"|"				{ return('|'); }
"?"				{ return('?'); }

[ \t\v\n\f]		{ }
.			{ /* ignore bad characters */ }

%%

int check_type()
{
	return(IDENTIFIER);
}

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}

// TODO : add comment
