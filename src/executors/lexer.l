%{
#include "../include/lexer_utils.hpp"
#include "../include/parser_utils.hpp"
#include "parser.yy.h"
#include <string>

using namespace std;

// Forward declarations
extern "C" int yylex(void);
extern FILE* yyin;

// Update column for each token
#define UPDATE_POS col_num += yyleng;

// Add token to tokens vector
#define ADD_TOKEN(TYPE, VALUE) tokens.emplace_back(Tokens{TYPE, VALUE, line_num, col_num - yyleng}); \
                               cout << TYPE << ": " << VALUE << endl;

// Add unknown token to unknown_tokens vector
#define ADD_UNKNOWN_TOKEN(VALUE) unknown_tokens.emplace_back(UnknownTokens{VALUE, line_num, col_num - yyleng}); \
                                 cout << "Unknown: " << VALUE << " at line " << line_num << endl;
%}

%x DEFINITION INCLUDE
%option noyywrap
%option never-interactive

DIGIT      [0-9]
ID         [a-zA-Z_][a-zA-Z0-9_]*
WS         [ \t\r]+
NL         \n
ESC        \\[abfnrtv\\'"?]
CHAR       \'(\\.|[^\\'])*\'
STR        \"(\\.|[^\\"])*\"
HEX        0[xX][0-9a-fA-F]+
OCT        0[0-7]+
FLOAT      {DIGIT}+\.{DIGIT}*([eE][+-]?{DIGIT}+)?
INT        {DIGIT}+

%%

"//".*                     { /* Single-line comment, no column update */ }
"/*"([^*]|\*+[^*/])*\*+\/ { /* Multi-line comment, no column update */ }

{WS}                      { /* Ignore whitespace, no column update */ }
{NL}                      { line_num++; col_num = 1; /* Reset column on newline */ }

"#define"[ \t]*           { UPDATE_POS; BEGIN(DEFINITION); }
<DEFINITION>{ID}[ \t]+[^ \t\n]+ { 
    UPDATE_POS;
    std::string text(yytext);
    size_t space = text.find_first_of(" \t");
    std::string name = text.substr(0, space);
    std::string value = text.substr(space + 1);
    define_macro(name, value);
    ADD_TOKEN("Preprocessor", "#define " + name + " " + value);
    BEGIN(INITIAL); 
}
<DEFINITION>.|\n          { 
    UPDATE_POS;
    ADD_UNKNOWN_TOKEN(std::string(yytext));
    std::cerr << "Invalid macro at line " << line_num << std::endl; 
    BEGIN(INITIAL); 
}

"#include"[ \t]*           { UPDATE_POS; BEGIN(INCLUDE); }
<INCLUDE>\"[^"\n]+\"      { 
    UPDATE_POS;
    include_file(std::string(yytext), false);
    ADD_TOKEN("Preprocessor", "#include " + std::string(yytext));
    BEGIN(INITIAL); 
}
<INCLUDE>\<[^>\n]+>       { 
    UPDATE_POS;
    include_file(std::string(yytext).substr(1, std::string(yytext).length() - 2), true);
    ADD_TOKEN("Preprocessor", "#include " + std::string(yytext));
    BEGIN(INITIAL); 
}
<INCLUDE>.|\n             { 
    UPDATE_POS;
    ADD_UNKNOWN_TOKEN(std::string(yytext));
    std::cerr << "Invalid include at line " << line_num << std::endl; 
    BEGIN(INITIAL); 
}

"#ifdef"                  { UPDATE_POS; ADD_TOKEN("Preprocessor", "#ifdef"); }
"#ifndef"                 { UPDATE_POS; ADD_TOKEN("Preprocessor", "#ifndef"); }
"#else"                   { UPDATE_POS; ADD_TOKEN("Preprocessor", "#else"); }
"#endif"                  { UPDATE_POS; ADD_TOKEN("Preprocessor", "#endif"); }
"#undef"                  { UPDATE_POS; ADD_TOKEN("Preprocessor", "#undef"); }
"#pragma"                 { UPDATE_POS; ADD_TOKEN("Preprocessor", "#pragma"); }

"auto"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"break"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"case"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"char"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"const"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"continue"                { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"default"                 { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"do"                      { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"double"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"else"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"enum"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"extern"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"float"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"for"                     { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"goto"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"if"                      { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"int"                     { UPDATE_POS; ADD_TOKEN("Keyword", yytext); return INT; }
"long"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"register"                { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"return"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); return RETURN; }
"short"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"signed"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"sizeof"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"static"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"struct"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"switch"                  { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"typedef"                 { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"union"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"unsigned"                { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"void"                    { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"volatile"                { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }
"while"                   { UPDATE_POS; ADD_TOKEN("Keyword", yytext); }

"=="|"!="|"<="|">="|">"|"<" { UPDATE_POS; ADD_TOKEN("Relational Operator", yytext); }
"="|"+"|"-"|"*"|"/"|"%"|"^"|"."|"++"|"--"|"&&"|"||"|"&"|"|"|"~"|"<<"|">>"|"->"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"|="|"<<="|">>=" { UPDATE_POS; ADD_TOKEN("Operator", yytext); }

"("                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); return LPAREN; }
")"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); return RPAREN; }
"{"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); return LBRACE; }
"}"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); return RBRACE; }
";"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); return SEMICOLON; }
","                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); }
"["                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); }
"]"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); }
":"                       { UPDATE_POS; ADD_TOKEN("Punctuation", yytext); }

{FLOAT}                   { UPDATE_POS; ADD_TOKEN("Float", yytext); }
{HEX}                     { UPDATE_POS; ADD_TOKEN("Hex", yytext); }
{OCT}                     { UPDATE_POS; ADD_TOKEN("Octal", yytext); }
{INT}                     { UPDATE_POS; ADD_TOKEN("Int", yytext); yylval.str = new std::string(yytext); return NUMBER; }

{CHAR}                    { UPDATE_POS; ADD_TOKEN("Char", yytext); }
{STR}                     { UPDATE_POS; ADD_TOKEN("String", yytext); yylval.str = new std::string(yytext); return STRING; }

{ID}                      { 
    UPDATE_POS;
    std::string expanded = expand_macro(yytext);
    if (expanded != yytext) {
        ADD_TOKEN("Macro Expansion", expanded);
        yylval.str = new std::string(expanded);
        return NUMBER;
    } else {
        ADD_TOKEN("Identifier", yytext);
        yylval.str = new std::string(yytext);
        return IDENTIFIER;
    }
}

.                         { 
    UPDATE_POS;
    ADD_UNKNOWN_TOKEN(std::string(yytext));
}

%%