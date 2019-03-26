/* Modified from reference on https://www.lysator.liu.se/c/ANSI-C-grammar-l.html */
%option noyywrap

%{
extern "C" int fileno(FILE *stream);
#include "Translator_parser.tab.hpp"

%}

D		[0-9]
L		[a-zA-Z_]
H		[a-fA-F0-9]
E		[Ee][+-]?{D}+
FS		(f|F|l|L)
IS		(u|U|l|L)*

%%

"else"			{ return(T_ELSE); }
"return"		{ return(T_RETURN); }
"if"			{ return(T_IF); }
"int"			{ return(T_INT); }
"return"		{ return(T_RETURN); }
"void"			{ return(T_VOID); }
"while"			{ return(T_WHILE); }

{L}({L}|{D})* { zzlval.string = new std::string(zztext);
				return(IDENTIFIER); }

0[xX]{H}+{IS}?		{ zzlval.string = new std::string(zztext);
					  return(CONSTANT); }
0{D}+{IS}?			{ zzlval.string = new std::string(zztext);
					  return(CONSTANT); }
{D}+{IS}?			{ zzlval.string = new std::string(zztext);
					  return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ zzlval.string = new std::string(zztext);
					  return(CONSTANT); }

{D}+{E}{FS}?			{ return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ return(CONSTANT); }


"&&"			{ return(AND_OP); }
"||"			{ return(OR_OP); }
"=="			{ return(EQ_OP); }
";"				{ return(';'); }
"="				{ return('='); }
"("				{ return('('); }
")"				{ return(')'); }
("{"|"<%")		{ return('{'); }
("}"|"%>")		{ return('}'); }
"-"				{ return('-'); }
"+"				{ return('+'); }
"*"				{ return('*'); }
"<"				{ return('<'); }
">"				{ return('>'); }
","				{ return(','); }


[ \t\v\n\f]		{  }
.			{ fprintf(stderr, "Invalid token\n"); exit(1);}

%%


int check_ptype()
{
	return(IDENTIFIER);
}


void zzerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
