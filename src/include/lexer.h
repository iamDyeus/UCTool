#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

    int yylex(void);
    int yyparse(void);
    void yyerror(const char* msg);

#ifdef __cplusplus
}
#endif

extern FILE* yyin;

#endif // LEXER_H