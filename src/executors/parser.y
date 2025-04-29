%{
#include <iostream>
#include <regex>
#include <string>
#include "../include/lexer_utils.hpp"
#include "../include/parser_utils.hpp"
#include "../include/lexer.h"
#include "parser.yy.h"

extern ProgramNode* parse_result;
extern TokenIterator* token_iterator;

#define YYLEX_PARAM token_iterator
#define yylex() custom_yylex(YYLEX_PARAM)

int custom_yylex(TokenIterator* iter) {
    if (!iter->has_next()) return 0; // EOF
    Tokens* token = iter->next();
    if (token->type == "Unknown") {
        std::cerr << "Unknown token: " << token->value << " at line " << token->line_no << "\n";
        return -1; // Error
    }

    std::cout << "Processing token: " << token->type << ", Value: " << token->value << "\n"; // Debug
    if (token->type == "Keyword") {
        if (token->value == "int") return INT;
        if (token->value == "return") return RETURN;
        if (token->value == "float") return FLOAT;
        return -1; // Other keywords not parsed yet
    } else if (token->type == "Identifier") {
        yylval.str = new std::string(token->value);
        return IDENTIFIER;
    } else if (token->type == "Punctuation") {
        if (token->value == "(") return LPAREN;
        if (token->value == ")") return RPAREN;
        if (token->value == "{") return LBRACE;
        if (token->value == "}") return RBRACE;
        if (token->value == ";") return SEMICOLON;
        return -1; // Other punctuation not parsed
    } else if (token->type == "String") {
        yylval.str = new std::string(token->value);
        return STRING;
    } else if (token->type == "Int" || token->type == "Float" || token->type == "Hex" || token->type == "Octal") {
        yylval.str = new std::string(token->value);
        return NUMBER;
    } else if (token->type == "Macro Expansion") {
        yylval.str = new std::string(token->value);
        return NUMBER; // Treat as NUMBER (e.g., MAX -> 10)
    } else if (token->type == "Preprocessor") {
        if (std::regex_match(token->value, std::regex("^#include\\s*[<\"][^>\"]+[>\"]\\s*$"))) {
            std::cout << "Skipping #include: " << token->value << "\n";
            return custom_yylex(iter); // Skip and get next token
        }
        yylval.str = new std::string(token->value);
        return PREPROCESSOR;
    } else if (token->type == "Operator") {
        if (token->value == "=") {
            yylval.str = new std::string(token->value);
            return ASSIGN;
        }
        return -1; // Other operators not parsed
    }
    return -1; // Fallback
}

void yyerror(const char* msg) {
    std::cerr << "Parse error: " << msg << "\n";
}
%}

%define parse.trace
%verbose

%union {
    std::string* str;
    ASTNode* node;
    ProgramNode* program;
    FunctionNode* function;
    StatementNode* statement;
    std::vector<StatementNode*>* decl_list;
}

%token INT RETURN FLOAT ASSIGN
%token <str> PREPROCESSOR IDENTIFIER STRING NUMBER
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON

%type <program> program
%type <function> function
%type <statement> preprocessor_list statement_list statement declaration
%type <decl_list> declaration_list

%start program

%%
program
    : preprocessor_list declaration_list function { 
        std::cout << "Building ProgramNode\n"; // Debug
        $$ = new ProgramNode(); 
        if ($3 && !$3->name.empty()) {
            $$->functions.push_back($3); 
        } else {
            delete $3;
        }
        if ($1 && !$1->children.empty()) {
            for (const auto* node : $1->children) {
                if (node && !node->value.empty()) {
                    $$->children.push_back(new ASTNode(*node)); // Copy to avoid double-delete
                }
            }
        }
        // Do not delete $1; its children are now owned by $$->children
        if ($2 && !$2->empty()) {
            for (const auto* decl : *$2) {
                if (decl && !decl->value.empty()) {
                    $$->children.push_back(new ASTNode(decl->type, decl->value)); // Copy to avoid double-delete
                }
            }
        }
        // Do not delete $2; its elements are now owned by $$->children
        delete $2; // Delete the vector, not its elements
        parse_result = $$; 
        std::cout << "ProgramNode built\n"; // Debug
    }
    ;

preprocessor_list
    : /* empty */ { $$ = new StatementNode(); $$->type = "PreprocessorList"; }
    | preprocessor_list PREPROCESSOR { 
        std::cout << "Building PreprocessorList with: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = $1 ? $1 : new StatementNode();
        $$->type = "PreprocessorList";
        if ($2 && !$2->empty()) {
            $$->children.push_back(new ASTNode("Preprocessor", *$2)); 
        }
        delete $2; 
    }
    ;

declaration_list
    : /* empty */ { $$ = new std::vector<StatementNode*>(); }
    | declaration_list declaration { 
        $$ = $1 ? $1 : new std::vector<StatementNode*>();
        if ($2 && !$2->value.empty()) {
            $$->push_back($2);
        } else {
            delete $2;
        }
    }
    ;

function
    : INT IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE
      { 
        std::cout << "Building FunctionNode: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new FunctionNode(); 
        $$->return_type = "int"; 
        $$->name = $2 && !$2->empty() ? *$2 : "unknown"; 
        $$->statements = $6 && !$6->statements.empty() ? $6->statements : std::vector<StatementNode*>(); 
        delete $2; 
        // Do not delete $6; its statements are now owned by $$->statements
        std::cout << "FunctionNode built with " << $$->statements.size() << " statements\n"; // Debug
      }
    ;

statement_list
    : /* empty */ { $$ = new StatementNode(); $$->type = "Empty"; }
    | statement_list statement { 
        $$ = $1 ? $1 : new StatementNode();
        $$->type = "StatementList";
        if ($2 && !$2->value.empty()) {
            $$->statements.push_back($2); 
        } else {
            delete $2;
        }
    }
    ;

statement
    : IDENTIFIER LPAREN STRING RPAREN SEMICOLON
      { 
        std::cout << "Building Call: " << ($1 ? *$1 : "null") << ", " << ($3 ? *$3 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Call"; 
        std::string id = ($1 && !$1->empty()) ? *$1 : "invalid";
        std::string arg = ($3 && !$3->empty()) ? *$3 : "\"\"";
        $$->value = id + "(" + arg + ")";
        delete $1; 
        delete $3; 
      }
    | RETURN NUMBER SEMICOLON
      { 
        std::cout << "Building Return: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Return"; 
        $$->value = ($2 && !$2->empty()) ? *$2 : "0";
        delete $2; 
      }
    ;

declaration
    : FLOAT IDENTIFIER ASSIGN NUMBER SEMICOLON
      { 
        std::cout << "Building Declaration: " << ($2 ? *$2 : "null") << ", " << ($4 ? *$4 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Declaration"; 
        std::string id = ($2 && !$2->empty()) ? *$2 : "unknown";
        std::string val = ($4 && !$4->empty()) ? *$4 : "0.0";
        $$->value = "float " + id + " = " + val;
        delete $2; 
        delete $4; 
      }
    ;
%%