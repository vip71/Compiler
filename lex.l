%option noyywrap
%option yylineno
%option nounput

%{
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
    #include "parser.tab.h"
	unsigned long long lineno = 1;
	int errorInLine = 0;
	int errorOccurred = 0;
	void yyerror(const char *s) {
        fprintf(stderr, "\033[31m[Error]   in line %llu: %s\033[0m\n",lineno, s);
        errorInLine=1;
        errorOccurred=1;
    }
    void yywarning(const char *s) {
        if(errorInLine==0){
        fprintf(stderr, "\033[33m[Warning] in line %llu: %s\033[0m\n",lineno, s);
        }
        }
    #include "myInstructions.hh"
    int yylex();
%}

%%
\n		{lineno++;errorInLine=0;}
\#.*\n		{lineno++;errorInLine=0;}
[ \t\r]+      {}

"+"			{return '+';}
"-"			{return '-';}
"*"			{return '*';}
"/"			{return '/';}
"%"			{return '%';}
"="			{return '=';}
"<"			{return '<';}
">"			{return '>';}
"!="		{return NEQ_T;}
"<="		{return LT_T;}
">="		{return GT_T;}
":="		{return ASSIGN_T;}

PROCEDURE   {return PROCEDURE_T;}
PROGRAM     {return PROGRAM_T;}
IS          {return IS_T;}
IN          {return IN_T;}
END          {return END_T;}

READ		{return READ_T;}
WRITE		{return WRITE_T;}

IF			{return IF_T;}
THEN		{return THEN_T;}
ELSE		{return ELSE_T;}
ENDIF		{return ENDIF_T;}

WHILE		{return WHILE_T;}
DO 			{return DO_T;}
ENDWHILE	{return ENDWHILE_T;}

REPEAT      {return REPEAT_T;}
UNTIL       {return UNTIL_T;}

T           {return TABLE_T;}

";"			{return ';';}
"["			{return '[';}
"]"			{return ']';}
"("			{return '(';}
")"			{return ')';}
","			{return ',';}

[_a-z]+		    {yylval.string = strdup(yytext); return ID_T;}
[0-9]+	        {yylval.number = strtoull(yytext, NULL, 10); return NUM_T;}
[_a-zA-Z0-9]+	{yyerror("niepoprawny zapis"); yyterminate();}
.			    {yyerror("nieznany znak");yyerror(strdup(yytext)); yyterminate();}
%%
