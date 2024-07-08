%{
#include<stdio.h>
#include<stdlib.h>
#include"syntax.tab.h"
#include"node.h"

extern unsigned lexError;
int lines = 0;
int yycolumn = 1;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

%option yylineno
digit [0-9]
digits {digit}*
/*Decimal Octal Hexadecimal -> Integer*/
DEC ([1-9]{digit}{0,9}|0)
OCT (0[1-7]([0-7]){0,10}|0)
HEX (0x|0X)([1-9a-fA-F]([0-9a-fA-F]{0,3})|0)
INT {DEC}|{OCT}|{HEX}
/* Float */
FLOAT (([1-9]{digits}|0)"."{digits})|((({digits}"."{digit}+)|({digit}+"."{digits}))[Ee][+-]?{digit}+)
/* Punctuation */
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
BLANK " "|\n|\r|\t
/* Keywords */
STRUCT struct    
RETURN return
IF if
ELSE else
WHILE while
/* Comment */
LINE_COMMENT ("//"[.]*)
BLOCK_COMMENT "/\*"[.]*"\*/"
COMMENT {LINE_COMMENT}|{BLOCK_COMMENT}
/* Error */
INT8_ERROR 0[0-7]*[8-9]+[0-7]*
INT16_ERROR 0[Xx][a-fA-F0-9]*[g-zG-Z]+[a-fA-F0-9]*
FLOAT_ERROR [0]+(0|[1-9][0-9]*)\.[0-9]+|\.[0-9]+|[0-9]+\.|[0-9]+[Ee][+-]?[0-9]*|([+-]?([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)[Ee][+-]?[0-9]+\.[0-9]+)|[0-9]+\.[0-9]*[Ee]
NUM_ERROR {INT8_ERROR}|{INT16_ERROR}|{FLOAT_ERROR}
ID_ERROR [0-9]+[a-zA-Z_]+
/* ID at last to avoid conflict */
ID [_a-zA-Z][_0-9a-zA-Z]*
%%
"//" 		{ char c = input(); while (c != '\n') c = input(); }
"/*" {
    int q = 0;
    char c;
    while(1){
        if(q == 0){
            c = input();
            if(c == EOF)
                break;
            if(c == '*')
                q = 1;
        }else{
            c = input();
            if(c == EOF)
                break;
            if(c == '/')
                break;
            else if(c != '*')
                q = 0;
        }
    }
}

{INT8_ERROR}	{ lexError = TRUE; printf("Error type A at Line %d: Illegal octal number \"%s\".\n", yylineno, yytext); return INT8_ERROR;} 
{INT16_ERROR}	{ lexError = TRUE; printf("Error type A at Line %d: Illegal hexadecimal number \"%s\".\n", yylineno, yytext); return INT16_ERROR;}
{FLOAT_ERROR} 	{ lexError = TRUE; printf("Error type A at Line %d: Illegal floating point number \"%s\".\n", yylineno, yytext); return FLOAT_ERROR;}
{ID_ERROR}	{ lexError = TRUE; printf("Error type A at Line %d: Illegal ID \"%s\".\n", yylineno, yytext); return ID_ERROR;}

{INT} 		{ yylval.node = newTokenNode(yylineno, TOKEN_INT, "INT", yytext); return INT;}
{FLOAT} 	{ yylval.node = newTokenNode(yylineno, TOKEN_FLOAT, "FLOAT", yytext); return FLOAT;}
{SEMI} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "SEMI", yytext); return SEMI; }
{COMMA} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "COMMA", yytext); return COMMA; }
{ASSIGNOP} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "ASSIGNOP", yytext); return ASSIGNOP; }
{RELOP} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "RELOP", yytext); return RELOP; }
{PLUS} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "PLUS", yytext); return PLUS; }
{MINUS} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "MINUS", yytext); return MINUS; }
{STAR} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "STAR", yytext); return STAR; }
{DIV} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "DIV", yytext); return DIV; }
{AND} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "AND", yytext); return AND; }
{OR} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "OR", yytext); return OR; }
{DOT} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "DOT", yytext); return DOT; }
{NOT} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "NOT", yytext); return NOT; }
{TYPE} 		{ yylval.node = newTokenNode(yylineno, TOKEN_TYPE, "TYPE", yytext); return TYPE; }
{LP} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "LP", yytext); return LP; }
{RP} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "RP", yytext); return RP; }
{LB} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "LB", yytext); return LB; }
{RB} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "RB", yytext); return RB; }
{LC} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "LC", yytext); return LC; }
{RC} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "RC", yytext); return RC; }
{IF} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "IF", yytext); return IF; }
{STRUCT} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "STRUCT", yytext); return STRUCT; }
{RETURN} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "RETURN", yytext); return RETURN; }
{ELSE} 		{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "ELSE", yytext); return ELSE; }
{WHILE} 	{ yylval.node = newTokenNode(yylineno, TOKEN_OTHER, "WHILE", yytext); return WHILE; }
{ID} 		{ yylval.node = newTokenNode(yylineno, TOKEN_ID, "ID", yytext); return ID;}
\n		{ yycolumn = 1;}
{BLANK} 	{}
. 		{ lexError = TRUE; printf("Error type A at Line %d: Mysterious character \'%s\'.\n", yylineno, yytext); }
%%

/*int main()
{
    while(yylex() != 0);
    return 0;
}*/
