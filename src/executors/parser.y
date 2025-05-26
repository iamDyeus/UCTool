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
        if (token->value == "void") return VOID;
        if (token->value == "if") return IF;
        if (token->value == "else") return ELSE;
        if (token->value == "for") return FOR;
        if (token->value == "while") return WHILE;
        if (token->value == "struct") return STRUCT;
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
        if (token->value == ",") return COMMA;
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
    } else if (token->type == "Operator" || token->type == "Relational Operator") {
        if (token->value == "=") return ASSIGN;
        if (token->value == ">") return GT;
        if (token->value == "<") return LT;
        if (token->value == "<=") return LE;
        if (token->value == "==") return EQ;
        if (token->value == "+") return PLUS;
        if (token->value == "-") return MINUS;
        if (token->value == "*") return MULT;
        if (token->value == "/") return DIV;
        if (token->value == "%") return MOD;
        if (token->value == "&") return ADDRESS;
        if (token->value == "++") return PLUSPLUS;
        if (token->value == "*=") return MULTEQ;
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
    std::vector<FunctionNode*>* func_list;
    std::vector<ASTNode*>* expr_list;
}

%token INT RETURN FLOAT VOID IF ELSE FOR WHILE STRUCT ASSIGN MULTEQ LE
%token GT LT EQ PLUS MINUS MULT DIV MOD ADDRESS PLUSPLUS
%token COMMA
%token <str> PREPROCESSOR IDENTIFIER STRING NUMBER
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON

%type <program> program
%type <function> function
%type <func_list> function_list
%type <statement> preprocessor_list statement_list statement declaration local_declaration if_statement for_statement while_statement struct_declaration assignment_statement expression_statement
%type <node> expression term incr_expression
%type <decl_list> declaration_list var_decls
%type <expr_list> expression_list

%left PLUS MINUS
%left MULT DIV MOD
%nonassoc GT LT EQ LE
%nonassoc ELSE

%start program

%%
program
    : preprocessor_list declaration_list function_list
      { 
        std::cout << "Building ProgramNode\n"; // Debug
        $$ = new ProgramNode(); 
        if ($3 && !$3->empty()) {
            $$->functions = *$3; // Transfer functions
        }
        if ($1 && !$1->children.empty()) {
            for (const auto* node : $1->children) {
                if (node && !node->value.empty()) {
                    $$->children.push_back(new ASTNode(*node));
                }
            }
        }
        if ($2 && !$2->empty()) {
            for (const auto* decl : *$2) {
                if (decl && !decl->value.empty()) {
                    $$->children.push_back(new ASTNode(decl->type, decl->value));
                }
            }
        }
        delete $2; // Delete declaration_list
        delete $3; // Delete function_list
        parse_result = $$; 
        std::cout << "ProgramNode built\n"; // Debug
      }
    ;

preprocessor_list
    : /* empty */ { $$ = new StatementNode(); $$->type = "PreprocessorList"; }
    | preprocessor_list PREPROCESSOR
      { 
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
    | declaration_list declaration
      { 
        std::cout << "Adding declaration to declaration_list\n"; // Debug
        $$ = $1 ? $1 : new std::vector<StatementNode*>();
        if ($2 && !($2->value.empty())) {
            $$->push_back($2);
        } else {
            delete $2;
        }
      }
    | declaration_list struct_declaration
      { 
        std::cout << "Adding struct_declaration to declaration_list\n"; // Debug
        $$ = $1 ? $1 : new std::vector<StatementNode*>();
        if ($2 && !($2->value.empty())) {
            $$->push_back($2);
        } else {
            delete $2;
        }
      }
    ;

function_list
    : /* empty */ { $$ = new std::vector<FunctionNode*>(); }
    | function_list function
      { 
        std::cout << "Adding function to function_list\n"; // Debug
        $$ = $1 ? $1 : new std::vector<FunctionNode*>();
        if ($2 && !$2->name.empty()) {
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
        std::cout << "FunctionNode built with " << $$->statements.size() << " statements\n"; // Debug
      }
    | VOID IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE
      { 
        std::cout << "Building FunctionNode: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new FunctionNode(); 
        $$->return_type = "void"; 
        $$->name = $2 && !$2->empty() ? *$2 : "unknown"; 
        $$->statements = $6 && !$6->statements.empty() ? $6->statements : std::vector<StatementNode*>(); 
        delete $2; 
        std::cout << "FunctionNode built with " << $$->statements.size() << " statements\n"; // Debug
      }
    ;

statement_list
    : /* empty */ { $$ = new StatementNode(); $$->type = "Empty"; }
    | statement_list statement
      { 
        std::cout << "Adding statement to statement_list\n"; // Debug
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
    : IDENTIFIER LPAREN expression_list RPAREN SEMICOLON
      { 
        std::cout << "Building Call: " << ($1 ? *$1 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Call"; 
        std::string id = ($1 && !$1->empty()) ? *$1 : "invalid";
        std::string args = "";
        if ($3 && !$3->empty()) {
            for (size_t i = 0; i < $3->size(); ++i) {
                args += (*$3)[i]->value;
                if (i < $3->size() - 1) args += ", ";
            }
        }
        $$->value = id + "(" + args + ")";
        if ($3) {
            $$->children = *$3; // Transfer ownership
            delete $3; // Delete the vector
        }
        delete $1; 
      }
    | RETURN expression SEMICOLON
      { 
        std::cout << "Building Return: " << ($2 ? $2->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Return"; 
        $$->value = $2 ? $2->value : "0";
        $$->children.push_back($2); 
      }
    | expression_statement { $$ = $1; }
    | if_statement { $$ = $1; }
    | for_statement { $$ = $1; }
    | while_statement { $$ = $1; }
    | local_declaration { $$ = $1; }
    | assignment_statement { $$ = $1; }
    | struct_declaration { $$ = $1; }
    | declaration { $$ = $1; }
    ;

expression_statement
    : expression SEMICOLON
      { 
        std::cout << "Building Expression: " << ($1 ? $1->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Expression"; 
        $$->value = $1 ? $1->value : "unknown";
        $$->children.push_back($1); 
      }
    ;

declaration
    : FLOAT IDENTIFIER SEMICOLON
      { 
        std::cout << "Building Declaration: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Declaration"; 
        std::string id = ($2 && !$2->empty()) ? *$2 : "unknown";
        $$->value = "float " + id;
        delete $2; 
      }
    | FLOAT IDENTIFIER ASSIGN expression SEMICOLON
      { 
        std::cout << "Building Declaration: " << ($2 ? *$2 : "null") << ", " << ($4 ? $4->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Declaration"; 
        std::string id = ($2 && !$2->empty()) ? *$2 : "unknown";
        std::string val = ($4 && !$4->value.empty()) ? $4->value : "0.0";
        $$->value = "float " + id + " = " + val;
        $$->children.push_back($4); 
        delete $2; 
      }
    ;

local_declaration
    : INT var_decls SEMICOLON
      { 
        std::cout << "Building Local Declaration\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "LocalDeclaration"; 
        $$->value = "int declarations";
        if ($2 && !$2->empty()) {
            $$->statements = *$2; // Transfer declarations
        }
        delete $2; 
      }
    ;

var_decls
    : IDENTIFIER
      { 
        std::cout << "Building VarDecl: " << ($1 ? *$1 : "null") << "\n"; // Debug
        $$ = new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        decl->value = "int " + ($1 ? *$1 : "unknown");
        $$->push_back(decl);
        delete $1; 
      }
    | IDENTIFIER ASSIGN expression
      { 
        std::cout << "Building VarDecl: " << ($1 ? *$1 : "null") << ", " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        std::string id = ($1 && !$1->empty()) ? *$1 : "unknown";
        std::string val = ($3 && !$3->value.empty()) ? $3->value : "0";
        decl->value = "int " + id + " = " + val;
        decl->children.push_back($3);
        $$->push_back(decl);
        delete $1; 
      }
    | var_decls COMMA IDENTIFIER
      { 
        std::cout << "Building VarDecl: " << ($3 ? *$3 : "null") << "\n"; // Debug
        $$ = $1 ? $1 : new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        decl->value = "int " + ($3 ? *$3 : "unknown");
        $$->push_back(decl);
        delete $3; 
      }
    | var_decls COMMA IDENTIFIER ASSIGN expression
      { 
        std::cout << "Building VarDecl: " << ($3 ? *$3 : "null") << ", " << ($5 ? $5->value : "null") << "\n"; // Debug
        $$ = $1 ? $1 : new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        std::string id = ($3 && !$3->empty()) ? *$3 : "unknown";
        std::string val = ($5 && !$5->value.empty()) ? $5->value : "0";
        decl->value = "int " + id + " = " + val;
        decl->children.push_back($5);
        $$->push_back(decl);
        delete $3; 
      }
    ;

if_statement
    : IF LPAREN expression RPAREN LBRACE statement_list RBRACE
      { 
        std::cout << "Building If: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "If"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($6 && !$6->statements.empty()) {
            $$->statements = $6->statements;
        }
      }
    | IF LPAREN expression RPAREN statement
      { 
        std::cout << "Building If: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "If"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($5 && !$5->value.empty()) {
            $$->statements.push_back($5);
        } else {
            delete $5;
        }
      }
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
      { 
        std::cout << "Building If-Else: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "IfElse"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($6 && !$6->statements.empty()) {
            $$->statements.insert($$->statements.end(), $6->statements.begin(), $6->statements.end());
        }
        if ($10 && !$10->statements.empty()) {
            $$->statements.insert($$->statements.end(), $10->statements.begin(), $10->statements.end());
        }
      }
    | IF LPAREN expression RPAREN statement ELSE statement
      { 
        std::cout << "Building If-Else: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "IfElse"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($5 && !$5->value.empty()) {
            $$->statements.push_back($5);
        } else {
            delete $5;
        }
        if ($7 && !$7->value.empty()) {
            $$->statements.push_back($7);
        } else {
            delete $7;
        }
      }
    ;

for_statement
    : FOR LPAREN local_declaration expression SEMICOLON incr_expression RPAREN LBRACE statement_list RBRACE
      { 
        std::cout << "Building For\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "For"; 
        $$->value = ($4 ? $4->value : "unknown");
        $$->children.push_back(new ASTNode("Init", $3->value)); 
        $$->children.push_back($4); 
        $$->children.push_back($6); 
        if ($9 && !$9->statements.empty()) {
            $$->statements = $9->statements;
        }
        delete $3; 
      }
    | FOR LPAREN local_declaration expression SEMICOLON incr_expression RPAREN statement
      { 
        std::cout << "Building For\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "For"; 
        $$->value = ($4 ? $4->value : "unknown");
        $$->children.push_back(new ASTNode("Init", $3->value)); 
        $$->children.push_back($4); 
        $$->children.push_back($6); 
        if ($8 && !$8->value.empty()) {
            $$->statements.push_back($8);
        } else {
            delete $8;
        }
        delete $3; 
      }
    ;

incr_expression
    : expression
      { $$ = $1; }
    | IDENTIFIER PLUSPLUS
      { 
        std::cout << "Building Increment: " << ($1 ? *$1 : "null") << "\n"; // Debug
        $$ = new ASTNode("Increment", ($1 ? *$1 : "unknown") + "++");
        $$->children.push_back(new ASTNode("Identifier", $1 ? *$1 : "unknown"));
        delete $1; 
      }
    | PLUSPLUS IDENTIFIER
      { 
        std::cout << "Building PreIncrement: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new ASTNode("PreIncrement", "++" + ($2 ? *$2 : "unknown"));
        $$->children.push_back(new ASTNode("Identifier", $2 ? *$2 : "unknown"));
        delete $2; 
      }
    ;

while_statement
    : WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE
      { 
        std::cout << "Building While: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "While"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($6 && !$6->statements.empty()) {
            $$->statements = $6->statements;
        }
      }
    | WHILE LPAREN expression RPAREN statement
      { 
        std::cout << "Building While: " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "While"; 
        $$->value = $3 ? $3->value : "unknown";
        $$->children.push_back($3); 
        if ($5 && !$5->value.empty()) {
            $$->statements.push_back($5);
        } else {
            delete $5;
        }
      }
    ;

struct_declaration
    : STRUCT IDENTIFIER LBRACE declaration_list RBRACE SEMICOLON
      { 
        std::cout << "Building Struct: " << ($2 ? *$2 : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Struct"; 
        std::string id = ($2 && !$2->empty()) ? *$2 : "unknown";
        $$->value = "struct " + id;
        if ($4 && !$4->empty()) {
            for (const auto* decl : *$4) {
                if (decl && !decl->value.empty()) {
                    $$->children.push_back(new ASTNode(decl->type, decl->value));
                }
            }
        }
        delete $2; 
        delete $4; // Delete the vector
      }
    ;

assignment_statement
    : IDENTIFIER ASSIGN expression SEMICOLON
      { 
        std::cout << "Building Assignment: " << ($1 ? *$1 : "null") << ", " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Assignment"; 
        std::string id = ($1 && !$1->empty()) ? *$1 : "unknown";
        std::string val = ($3 && !$3->value.empty()) ? $3->value : "0";
        $$->value = id + " = " + val;
        $$->children.push_back($3); 
        delete $1; 
      }
    | IDENTIFIER MULTEQ expression SEMICOLON
      { 
        std::cout << "Building Assignment: " << ($1 ? *$1 : "null") << ", " << ($3 ? $3->value : "null") << "\n"; // Debug
        $$ = new StatementNode(); 
        $$->type = "Assignment"; 
        std::string id = ($1 && !$1->empty()) ? *$1 : "unknown";
        std::string val = ($3 && !$3->value.empty()) ? $3->value : "0";
        $$->value = id + " *= " + val;
        $$->children.push_back($3); 
        delete $1; 
      }
    ;

expression_list
    : /* empty */ { $$ = new std::vector<ASTNode*>(); }
    | expression
      { 
        $$ = new std::vector<ASTNode*>();
        if ($1 && !$1->value.empty()) {
            $$->push_back($1);
        } else {
            delete $1;
        }
      }
    | expression_list COMMA expression
      { 
        $$ = $1 ? $1 : new std::vector<ASTNode*>();
        if ($3 && !$3->value.empty()) {
            $$->push_back($3);
        } else {
            delete $3;
        }
      }
    ;

expression
    : term { $$ = $1; }
    | expression PLUS term
      { 
        $$ = new ASTNode("Add", ($1 ? $1->value : "0") + " + " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression MINUS term
      { 
        $$ = new ASTNode("Subtract", ($1 ? $1->value : "0") + " - " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression MULT term
      { 
        $$ = new ASTNode("Multiply", ($1 ? $1->value : "0") + " * " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression DIV term
      { 
        $$ = new ASTNode("Divide", ($1 ? $1->value : "0") + " / " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression MOD term
      { 
        $$ = new ASTNode("Modulo", ($1 ? $1->value : "0") + " % " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression GT term
      { 
        $$ = new ASTNode("Greater", ($1 ? $1->value : "0") + " > " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression LT term
      { 
        $$ = new ASTNode("Less", ($1 ? $1->value : "0") + " < " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression LE term
      { 
        $$ = new ASTNode("LessEqual", ($1 ? $1->value : "0") + " <= " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | expression EQ term
      { 
        $$ = new ASTNode("Equal", ($1 ? $1->value : "0") + " == " + ($3 ? $3->value : "0")); 
        $$->children.push_back($1);
        $$->children.push_back($3);
      }
    | ADDRESS term
      { 
        $$ = new ASTNode("Address", "&" + ($2 ? $2->value : "unknown")); 
        $$->children.push_back($2);
      }
    ;

term
    : IDENTIFIER
      { 
        $$ = new ASTNode("Identifier", $1 ? *$1 : "unknown");
        delete $1;
      }
    | NUMBER
      { 
        $$ = new ASTNode("Number", $1 ? *$1 : "0");
        delete $1;
      }
    | STRING
      { 
        $$ = new ASTNode("String", $1 ? *$1 : "\"\"");
        delete $1;
      }
    | LPAREN expression RPAREN { $$ = $2; }
    | IDENTIFIER PLUSPLUS
      { 
        $$ = new ASTNode("Increment", ($1 ? *$1 : "unknown") + "++");
        $$->children.push_back(new ASTNode("Identifier", $1 ? *$1 : "unknown"));
        delete $1;
      }
    | PLUSPLUS IDENTIFIER
      { 
        $$ = new ASTNode("PreIncrement", "++" + ($2 ? *$2 : "unknown"));
        $$->children.push_back(new ASTNode("Identifier", $2 ? *$2 : "unknown"));
        delete $2;
      }
    ;
%%
