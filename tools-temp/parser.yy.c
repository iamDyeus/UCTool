/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "src/executors/parser.y"

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

#line 156 "tools-temp/parser.yy.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_TOOLS_TEMP_PARSER_YY_H_INCLUDED
# define YY_YY_TOOLS_TEMP_PARSER_YY_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT = 258,
    RETURN = 259,
    FLOAT = 260,
    VOID = 261,
    IF = 262,
    ELSE = 263,
    FOR = 264,
    WHILE = 265,
    STRUCT = 266,
    ASSIGN = 267,
    MULTEQ = 268,
    LE = 269,
    GT = 270,
    LT = 271,
    EQ = 272,
    PLUS = 273,
    MINUS = 274,
    MULT = 275,
    DIV = 276,
    MOD = 277,
    ADDRESS = 278,
    PLUSPLUS = 279,
    COMMA = 280,
    PREPROCESSOR = 281,
    IDENTIFIER = 282,
    STRING = 283,
    NUMBER = 284,
    LPAREN = 285,
    RPAREN = 286,
    LBRACE = 287,
    RBRACE = 288,
    SEMICOLON = 289
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 90 "src/executors/parser.y"

    std::string* str;
    ASTNode* node;
    ProgramNode* program;
    FunctionNode* function;
    StatementNode* statement;
    std::vector<StatementNode*>* decl_list;
    std::vector<FunctionNode*>* func_list;
    std::vector<ASTNode*>* expr_list;

#line 254 "tools-temp/parser.yy.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOOLS_TEMP_PARSER_YY_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   414

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  35
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  142

#define YYUNDEFTOK  2
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   124,   124,   153,   154,   167,   168,   178,   191,   192,
     205,   215,   228,   229,   243,   263,   271,   272,   273,   274,
     275,   276,   277,   278,   282,   293,   302,   316,   330,   340,
     353,   363,   379,   390,   403,   417,   438,   452,   471,   473,
     480,   490,   501,   517,   537,   548,   562,   563,   572,   584,
     585,   591,   597,   603,   609,   615,   621,   627,   633,   639,
     647,   652,   657,   662,   663,   669
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "RETURN", "FLOAT", "VOID", "IF",
  "ELSE", "FOR", "WHILE", "STRUCT", "ASSIGN", "MULTEQ", "LE", "GT", "LT",
  "EQ", "PLUS", "MINUS", "MULT", "DIV", "MOD", "ADDRESS", "PLUSPLUS",
  "COMMA", "PREPROCESSOR", "IDENTIFIER", "STRING", "NUMBER", "LPAREN",
  "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "$accept", "program",
  "preprocessor_list", "declaration_list", "function_list", "function",
  "statement_list", "statement", "expression_statement", "declaration",
  "local_declaration", "var_decls", "if_statement", "for_statement",
  "incr_expression", "while_statement", "struct_declaration",
  "assignment_statement", "expression_list", "expression", "term", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289
};
# endif

#define YYPACT_NINF (-61)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-41)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -61,     3,    -7,   -61,   -61,    -3,     7,    21,    41,   -61,
     -61,    27,    18,    24,    25,   -61,   260,   -61,   -61,    33,
      37,   211,    30,    44,   -61,   -61,   260,   248,   -61,    22,
      39,    49,   -61,   -61,   -61,   338,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   -61,    52,    57,    58,   -61,
     -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,
     -61,   -61,     2,    13,    60,   260,    63,    65,    67,    79,
     -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,   -61,
     257,   -61,    86,    20,   278,   260,   102,   260,   260,   260,
     260,   -61,   260,    83,   -61,   -61,   356,   260,   374,   287,
     308,   -10,   392,   392,   104,   150,   317,   180,   -61,   -61,
     260,    80,   260,   -61,   114,   290,   -61,   -61,   392,   -61,
     392,    55,   220,    96,   105,   103,   392,    97,   120,   -61,
     109,   116,   190,   -61,   101,   -61,   -61,   -61,   108,   139,
     -61,   -61
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     5,     1,     4,     8,     0,     0,     2,     6,
       7,     0,     0,     0,     0,     9,     0,    25,     5,     0,
       0,     0,     0,    60,    62,    61,     0,     0,    49,     0,
       0,     0,    59,    65,    64,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,     0,     0,     0,    63,
      57,    55,    56,    58,    50,    51,    52,    53,    54,    43,
      12,    12,     0,     0,     0,     0,     0,     0,     0,    60,
      10,    13,    16,    23,    20,    17,    18,    19,    22,    21,
       0,    11,    28,     0,     0,     0,     0,     0,     0,     0,
      46,    24,     0,     0,    27,    15,     0,     0,     0,     0,
       0,     0,    47,    29,    30,     0,     0,     0,    44,    45,
       0,     0,     0,    12,    33,     0,    12,    42,    48,    14,
      31,     0,     0,     0,    60,     0,    38,     0,    32,    35,
      65,    64,     0,    41,     0,    12,    37,    12,     0,     0,
      36,    34
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -61,   -61,   -61,   121,   -61,   -61,   -60,   129,   -61,    -1,
      59,   -61,   -61,   -61,   -61,   -61,     9,   -61,   -61,   -16,
     216
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     5,     8,    15,    62,    71,    72,    73,
      74,    83,    75,    76,   125,    77,    78,    79,   101,    80,
      28
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      27,    63,     6,     3,     9,    64,    65,     6,     7,    66,
      35,    67,    68,     7,    10,   110,    64,    65,     6,     4,
      66,   111,    67,    68,     7,    21,    22,     6,     9,    69,
      24,    25,    26,     7,    11,    70,    21,    22,    10,    16,
      69,    24,    25,    26,    13,    93,    81,    14,    12,    84,
      18,    19,    20,   121,    94,    46,   127,    33,    64,    65,
       6,    17,    66,    30,    67,    68,     7,    31,    34,    96,
      47,    98,    99,   100,   102,   138,   103,   139,    21,    22,
      48,   106,    69,    24,    25,    26,    59,    82,   128,    60,
      61,    88,    89,    85,   118,    86,   120,    87,    92,   126,
      64,    65,     6,    34,    66,    64,    67,    68,     7,    90,
     104,    64,    65,     6,   119,    66,   112,    67,    68,     7,
      21,    22,   122,   130,    69,    24,    25,    26,   134,   131,
     133,    21,    22,   137,   132,    69,    24,    25,    26,    29,
     -40,   140,    64,    65,     6,    97,    66,   -39,    67,    68,
       7,     0,     0,    64,    65,     6,     0,    66,     0,    67,
      68,     7,    21,    22,     0,     0,    69,    24,    25,    26,
       0,     0,   141,    21,    22,     0,     0,    69,    24,    25,
      26,     0,   113,    64,    65,     6,     0,    66,     0,    67,
      68,     7,     0,    64,    65,     6,     0,    66,     0,    67,
      68,     7,     0,    21,    22,     0,     0,    69,    24,    25,
      26,     0,   116,    21,    22,     0,     0,    69,    24,    25,
      26,     0,   135,    64,    65,     6,     0,    66,     0,    67,
      68,     7,     0,     0,   114,    22,   117,    32,    23,    24,
      25,    26,     0,    21,    22,     0,     0,    69,    24,    25,
      26,   129,    50,    51,    52,    53,    54,    55,    56,    57,
      58,   136,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    36,    37,    38,    39,    40,    41,    42,    43,    44,
       0,     0,    45,    21,    22,     0,     0,    23,    24,    25,
      26,    91,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    36,    37,    38,    39,    40,    41,    42,    43,    44,
       0,     0,    95,    21,   123,     0,     0,   124,    24,    25,
      26,   108,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    36,    37,    38,    39,    40,    41,    42,    43,    44,
       0,     0,   109,     0,     0,     0,     0,     0,     0,     0,
       0,   115,    36,    37,    38,    39,    40,    41,    42,    43,
      44,     0,     0,     0,     0,     0,     0,     0,     0,    49,
      36,    37,    38,    39,    40,    41,    42,    43,    44,     0,
       0,     0,     0,     0,     0,     0,     0,   105,    36,    37,
      38,    39,    40,    41,    42,    43,    44,     0,     0,     0,
       0,     0,     0,     0,     0,   107,    36,    37,    38,    39,
      40,    41,    42,    43,    44
};

static const yytype_int16 yycheck[] =
{
      16,    61,     5,     0,     5,     3,     4,     5,    11,     7,
      26,     9,    10,    11,     5,    25,     3,     4,     5,    26,
       7,    31,     9,    10,    11,    23,    24,     5,    29,    27,
      28,    29,    30,    11,    27,    33,    23,    24,    29,    12,
      27,    28,    29,    30,     3,    25,    33,     6,    27,    65,
      32,    27,    27,   113,    34,    33,   116,    27,     3,     4,
       5,    34,     7,    30,     9,    10,    11,    30,    24,    85,
      31,    87,    88,    89,    90,   135,    92,   137,    23,    24,
      31,    97,    27,    28,    29,    30,    34,    27,    33,    32,
      32,    12,    13,    30,   110,    30,   112,    30,    12,   115,
       3,     4,     5,    24,     7,     3,     9,    10,    11,    30,
      27,     3,     4,     5,    34,     7,    12,     9,    10,    11,
      23,    24,     8,    27,    27,    28,    29,    30,     8,    24,
      33,    23,    24,    32,    31,    27,    28,    29,    30,    18,
      31,    33,     3,     4,     5,    86,     7,    31,     9,    10,
      11,    -1,    -1,     3,     4,     5,    -1,     7,    -1,     9,
      10,    11,    23,    24,    -1,    -1,    27,    28,    29,    30,
      -1,    -1,    33,    23,    24,    -1,    -1,    27,    28,    29,
      30,    -1,    32,     3,     4,     5,    -1,     7,    -1,     9,
      10,    11,    -1,     3,     4,     5,    -1,     7,    -1,     9,
      10,    11,    -1,    23,    24,    -1,    -1,    27,    28,    29,
      30,    -1,    32,    23,    24,    -1,    -1,    27,    28,    29,
      30,    -1,    32,     3,     4,     5,    -1,     7,    -1,     9,
      10,    11,    -1,    -1,   105,    24,   107,    21,    27,    28,
      29,    30,    -1,    23,    24,    -1,    -1,    27,    28,    29,
      30,   122,    36,    37,    38,    39,    40,    41,    42,    43,
      44,   132,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    34,    23,    24,    -1,    -1,    27,    28,    29,
      30,    34,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    34,    23,    24,    -1,    -1,    27,    28,    29,
      30,    34,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    31,    14,    15,    16,    17,
      18,    19,    20,    21,    22
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    36,    37,     0,    26,    38,     5,    11,    39,    44,
      51,    27,    27,     3,     6,    40,    12,    34,    32,    27,
      27,    23,    24,    27,    28,    29,    30,    54,    55,    38,
      30,    30,    55,    27,    24,    54,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    34,    33,    31,    31,    31,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    34,
      32,    32,    41,    41,     3,     4,     7,     9,    10,    27,
      33,    42,    43,    44,    45,    47,    48,    50,    51,    52,
      54,    33,    27,    46,    54,    30,    30,    30,    12,    13,
      30,    34,    12,    25,    34,    34,    54,    45,    54,    54,
      54,    53,    54,    54,    27,    31,    54,    31,    34,    34,
      25,    31,    12,    32,    42,    34,    32,    42,    54,    34,
      54,    41,     8,    24,    27,    49,    54,    41,    33,    42,
      27,    24,    31,    33,     8,    32,    42,    32,    41,    41,
      33,    33
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    35,    36,    37,    37,    38,    38,    38,    39,    39,
      40,    40,    41,    41,    42,    42,    42,    42,    42,    42,
      42,    42,    42,    42,    43,    44,    44,    45,    46,    46,
      46,    46,    47,    47,    47,    47,    48,    48,    49,    49,
      49,    50,    50,    51,    52,    52,    53,    53,    53,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      55,    55,    55,    55,    55,    55
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     0,     2,     0,     2,     2,     0,     2,
       7,     7,     0,     2,     5,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     5,     3,     1,     3,
       3,     5,     7,     5,    11,     7,    10,     8,     1,     2,
       2,     7,     5,     6,     4,     4,     0,     1,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       1,     1,     1,     3,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 125 "src/executors/parser.y"
      { 
        std::cout << "Building ProgramNode\n"; // Debug
        (yyval.program) = new ProgramNode(); 
        if ((yyvsp[0].func_list) && !(yyvsp[0].func_list)->empty()) {
            (yyval.program)->functions = *(yyvsp[0].func_list); // Transfer functions
        }
        if ((yyvsp[-2].statement) && !(yyvsp[-2].statement)->children.empty()) {
            for (const auto* node : (yyvsp[-2].statement)->children) {
                if (node && !node->value.empty()) {
                    (yyval.program)->children.push_back(new ASTNode(*node));
                }
            }
        }
        if ((yyvsp[-1].decl_list) && !(yyvsp[-1].decl_list)->empty()) {
            for (const auto* decl : *(yyvsp[-1].decl_list)) {
                if (decl && !decl->value.empty()) {
                    (yyval.program)->children.push_back(new ASTNode(decl->type, decl->value));
                }
            }
        }
        delete (yyvsp[-1].decl_list); // Delete declaration_list
        delete (yyvsp[0].func_list); // Delete function_list
        parse_result = (yyval.program); 
        std::cout << "ProgramNode built\n"; // Debug
      }
#line 1600 "tools-temp/parser.yy.c"
    break;

  case 3:
#line 153 "src/executors/parser.y"
                  { (yyval.statement) = new StatementNode(); (yyval.statement)->type = "PreprocessorList"; }
#line 1606 "tools-temp/parser.yy.c"
    break;

  case 4:
#line 155 "src/executors/parser.y"
      { 
        std::cout << "Building PreprocessorList with: " << ((yyvsp[0].str) ? *(yyvsp[0].str) : "null") << "\n"; // Debug
        (yyval.statement) = (yyvsp[-1].statement) ? (yyvsp[-1].statement) : new StatementNode();
        (yyval.statement)->type = "PreprocessorList";
        if ((yyvsp[0].str) && !(yyvsp[0].str)->empty()) {
            (yyval.statement)->children.push_back(new ASTNode("Preprocessor", *(yyvsp[0].str))); 
        }
        delete (yyvsp[0].str); 
      }
#line 1620 "tools-temp/parser.yy.c"
    break;

  case 5:
#line 167 "src/executors/parser.y"
                  { (yyval.decl_list) = new std::vector<StatementNode*>(); }
#line 1626 "tools-temp/parser.yy.c"
    break;

  case 6:
#line 169 "src/executors/parser.y"
      { 
        std::cout << "Adding declaration to declaration_list\n"; // Debug
        (yyval.decl_list) = (yyvsp[-1].decl_list) ? (yyvsp[-1].decl_list) : new std::vector<StatementNode*>();
        if ((yyvsp[0].statement) && !((yyvsp[0].statement)->value.empty())) {
            (yyval.decl_list)->push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 1640 "tools-temp/parser.yy.c"
    break;

  case 7:
#line 179 "src/executors/parser.y"
      { 
        std::cout << "Adding struct_declaration to declaration_list\n"; // Debug
        (yyval.decl_list) = (yyvsp[-1].decl_list) ? (yyvsp[-1].decl_list) : new std::vector<StatementNode*>();
        if ((yyvsp[0].statement) && !((yyvsp[0].statement)->value.empty())) {
            (yyval.decl_list)->push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 1654 "tools-temp/parser.yy.c"
    break;

  case 8:
#line 191 "src/executors/parser.y"
                  { (yyval.func_list) = new std::vector<FunctionNode*>(); }
#line 1660 "tools-temp/parser.yy.c"
    break;

  case 9:
#line 193 "src/executors/parser.y"
      { 
        std::cout << "Adding function to function_list\n"; // Debug
        (yyval.func_list) = (yyvsp[-1].func_list) ? (yyvsp[-1].func_list) : new std::vector<FunctionNode*>();
        if ((yyvsp[0].function) && !(yyvsp[0].function)->name.empty()) {
            (yyval.func_list)->push_back((yyvsp[0].function));
        } else {
            delete (yyvsp[0].function);
        }
      }
#line 1674 "tools-temp/parser.yy.c"
    break;

  case 10:
#line 206 "src/executors/parser.y"
      { 
        std::cout << "Building FunctionNode: " << ((yyvsp[-5].str) ? *(yyvsp[-5].str) : "null") << "\n"; // Debug
        (yyval.function) = new FunctionNode(); 
        (yyval.function)->return_type = "int"; 
        (yyval.function)->name = (yyvsp[-5].str) && !(yyvsp[-5].str)->empty() ? *(yyvsp[-5].str) : "unknown"; 
        (yyval.function)->statements = (yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty() ? (yyvsp[-1].statement)->statements : std::vector<StatementNode*>(); 
        delete (yyvsp[-5].str); 
        std::cout << "FunctionNode built with " << (yyval.function)->statements.size() << " statements\n"; // Debug
      }
#line 1688 "tools-temp/parser.yy.c"
    break;

  case 11:
#line 216 "src/executors/parser.y"
      { 
        std::cout << "Building FunctionNode: " << ((yyvsp[-5].str) ? *(yyvsp[-5].str) : "null") << "\n"; // Debug
        (yyval.function) = new FunctionNode(); 
        (yyval.function)->return_type = "void"; 
        (yyval.function)->name = (yyvsp[-5].str) && !(yyvsp[-5].str)->empty() ? *(yyvsp[-5].str) : "unknown"; 
        (yyval.function)->statements = (yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty() ? (yyvsp[-1].statement)->statements : std::vector<StatementNode*>(); 
        delete (yyvsp[-5].str); 
        std::cout << "FunctionNode built with " << (yyval.function)->statements.size() << " statements\n"; // Debug
      }
#line 1702 "tools-temp/parser.yy.c"
    break;

  case 12:
#line 228 "src/executors/parser.y"
                  { (yyval.statement) = new StatementNode(); (yyval.statement)->type = "Empty"; }
#line 1708 "tools-temp/parser.yy.c"
    break;

  case 13:
#line 230 "src/executors/parser.y"
      { 
        std::cout << "Adding statement to statement_list\n"; // Debug
        (yyval.statement) = (yyvsp[-1].statement) ? (yyvsp[-1].statement) : new StatementNode();
        (yyval.statement)->type = "StatementList";
        if ((yyvsp[0].statement) && !(yyvsp[0].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[0].statement)); 
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 1723 "tools-temp/parser.yy.c"
    break;

  case 14:
#line 244 "src/executors/parser.y"
      { 
        std::cout << "Building Call: " << ((yyvsp[-4].str) ? *(yyvsp[-4].str) : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Call"; 
        std::string id = ((yyvsp[-4].str) && !(yyvsp[-4].str)->empty()) ? *(yyvsp[-4].str) : "invalid";
        std::string args = "";
        if ((yyvsp[-2].expr_list) && !(yyvsp[-2].expr_list)->empty()) {
            for (size_t i = 0; i < (yyvsp[-2].expr_list)->size(); ++i) {
                args += (*(yyvsp[-2].expr_list))[i]->value;
                if (i < (yyvsp[-2].expr_list)->size() - 1) args += ", ";
            }
        }
        (yyval.statement)->value = id + "(" + args + ")";
        if ((yyvsp[-2].expr_list)) {
            (yyval.statement)->children = *(yyvsp[-2].expr_list); // Transfer ownership
            delete (yyvsp[-2].expr_list); // Delete the vector
        }
        delete (yyvsp[-4].str); 
      }
#line 1747 "tools-temp/parser.yy.c"
    break;

  case 15:
#line 264 "src/executors/parser.y"
      { 
        std::cout << "Building Return: " << ((yyvsp[-1].node) ? (yyvsp[-1].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Return"; 
        (yyval.statement)->value = (yyvsp[-1].node) ? (yyvsp[-1].node)->value : "0";
        (yyval.statement)->children.push_back((yyvsp[-1].node)); 
      }
#line 1759 "tools-temp/parser.yy.c"
    break;

  case 16:
#line 271 "src/executors/parser.y"
                           { (yyval.statement) = (yyvsp[0].statement); }
#line 1765 "tools-temp/parser.yy.c"
    break;

  case 17:
#line 272 "src/executors/parser.y"
                   { (yyval.statement) = (yyvsp[0].statement); }
#line 1771 "tools-temp/parser.yy.c"
    break;

  case 18:
#line 273 "src/executors/parser.y"
                    { (yyval.statement) = (yyvsp[0].statement); }
#line 1777 "tools-temp/parser.yy.c"
    break;

  case 19:
#line 274 "src/executors/parser.y"
                      { (yyval.statement) = (yyvsp[0].statement); }
#line 1783 "tools-temp/parser.yy.c"
    break;

  case 20:
#line 275 "src/executors/parser.y"
                        { (yyval.statement) = (yyvsp[0].statement); }
#line 1789 "tools-temp/parser.yy.c"
    break;

  case 21:
#line 276 "src/executors/parser.y"
                           { (yyval.statement) = (yyvsp[0].statement); }
#line 1795 "tools-temp/parser.yy.c"
    break;

  case 22:
#line 277 "src/executors/parser.y"
                         { (yyval.statement) = (yyvsp[0].statement); }
#line 1801 "tools-temp/parser.yy.c"
    break;

  case 23:
#line 278 "src/executors/parser.y"
                  { (yyval.statement) = (yyvsp[0].statement); }
#line 1807 "tools-temp/parser.yy.c"
    break;

  case 24:
#line 283 "src/executors/parser.y"
      { 
        std::cout << "Building Expression: " << ((yyvsp[-1].node) ? (yyvsp[-1].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Expression"; 
        (yyval.statement)->value = (yyvsp[-1].node) ? (yyvsp[-1].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-1].node)); 
      }
#line 1819 "tools-temp/parser.yy.c"
    break;

  case 25:
#line 294 "src/executors/parser.y"
      { 
        std::cout << "Building Declaration: " << ((yyvsp[-1].str) ? *(yyvsp[-1].str) : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Declaration"; 
        std::string id = ((yyvsp[-1].str) && !(yyvsp[-1].str)->empty()) ? *(yyvsp[-1].str) : "unknown";
        (yyval.statement)->value = "float " + id;
        delete (yyvsp[-1].str); 
      }
#line 1832 "tools-temp/parser.yy.c"
    break;

  case 26:
#line 303 "src/executors/parser.y"
      { 
        std::cout << "Building Declaration: " << ((yyvsp[-3].str) ? *(yyvsp[-3].str) : "null") << ", " << ((yyvsp[-1].node) ? (yyvsp[-1].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Declaration"; 
        std::string id = ((yyvsp[-3].str) && !(yyvsp[-3].str)->empty()) ? *(yyvsp[-3].str) : "unknown";
        std::string val = ((yyvsp[-1].node) && !(yyvsp[-1].node)->value.empty()) ? (yyvsp[-1].node)->value : "0.0";
        (yyval.statement)->value = "float " + id + " = " + val;
        (yyval.statement)->children.push_back((yyvsp[-1].node)); 
        delete (yyvsp[-3].str); 
      }
#line 1847 "tools-temp/parser.yy.c"
    break;

  case 27:
#line 317 "src/executors/parser.y"
      { 
        std::cout << "Building Local Declaration\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "LocalDeclaration"; 
        (yyval.statement)->value = "int declarations";
        if ((yyvsp[-1].decl_list) && !(yyvsp[-1].decl_list)->empty()) {
            (yyval.statement)->statements = *(yyvsp[-1].decl_list); // Transfer declarations
        }
        delete (yyvsp[-1].decl_list); 
      }
#line 1862 "tools-temp/parser.yy.c"
    break;

  case 28:
#line 331 "src/executors/parser.y"
      { 
        std::cout << "Building VarDecl: " << ((yyvsp[0].str) ? *(yyvsp[0].str) : "null") << "\n"; // Debug
        (yyval.decl_list) = new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        decl->value = "int " + ((yyvsp[0].str) ? *(yyvsp[0].str) : "unknown");
        (yyval.decl_list)->push_back(decl);
        delete (yyvsp[0].str); 
      }
#line 1876 "tools-temp/parser.yy.c"
    break;

  case 29:
#line 341 "src/executors/parser.y"
      { 
        std::cout << "Building VarDecl: " << ((yyvsp[-2].str) ? *(yyvsp[-2].str) : "null") << ", " << ((yyvsp[0].node) ? (yyvsp[0].node)->value : "null") << "\n"; // Debug
        (yyval.decl_list) = new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        std::string id = ((yyvsp[-2].str) && !(yyvsp[-2].str)->empty()) ? *(yyvsp[-2].str) : "unknown";
        std::string val = ((yyvsp[0].node) && !(yyvsp[0].node)->value.empty()) ? (yyvsp[0].node)->value : "0";
        decl->value = "int " + id + " = " + val;
        decl->children.push_back((yyvsp[0].node));
        (yyval.decl_list)->push_back(decl);
        delete (yyvsp[-2].str); 
      }
#line 1893 "tools-temp/parser.yy.c"
    break;

  case 30:
#line 354 "src/executors/parser.y"
      { 
        std::cout << "Building VarDecl: " << ((yyvsp[0].str) ? *(yyvsp[0].str) : "null") << "\n"; // Debug
        (yyval.decl_list) = (yyvsp[-2].decl_list) ? (yyvsp[-2].decl_list) : new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        decl->value = "int " + ((yyvsp[0].str) ? *(yyvsp[0].str) : "unknown");
        (yyval.decl_list)->push_back(decl);
        delete (yyvsp[0].str); 
      }
#line 1907 "tools-temp/parser.yy.c"
    break;

  case 31:
#line 364 "src/executors/parser.y"
      { 
        std::cout << "Building VarDecl: " << ((yyvsp[-2].str) ? *(yyvsp[-2].str) : "null") << ", " << ((yyvsp[0].node) ? (yyvsp[0].node)->value : "null") << "\n"; // Debug
        (yyval.decl_list) = (yyvsp[-4].decl_list) ? (yyvsp[-4].decl_list) : new std::vector<StatementNode*>();
        StatementNode* decl = new StatementNode();
        decl->type = "VarDecl";
        std::string id = ((yyvsp[-2].str) && !(yyvsp[-2].str)->empty()) ? *(yyvsp[-2].str) : "unknown";
        std::string val = ((yyvsp[0].node) && !(yyvsp[0].node)->value.empty()) ? (yyvsp[0].node)->value : "0";
        decl->value = "int " + id + " = " + val;
        decl->children.push_back((yyvsp[0].node));
        (yyval.decl_list)->push_back(decl);
        delete (yyvsp[-2].str); 
      }
#line 1924 "tools-temp/parser.yy.c"
    break;

  case 32:
#line 380 "src/executors/parser.y"
      { 
        std::cout << "Building If: " << ((yyvsp[-4].node) ? (yyvsp[-4].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "If"; 
        (yyval.statement)->value = (yyvsp[-4].node) ? (yyvsp[-4].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-4].node)); 
        if ((yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty()) {
            (yyval.statement)->statements = (yyvsp[-1].statement)->statements;
        }
      }
#line 1939 "tools-temp/parser.yy.c"
    break;

  case 33:
#line 391 "src/executors/parser.y"
      { 
        std::cout << "Building If: " << ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "If"; 
        (yyval.statement)->value = (yyvsp[-2].node) ? (yyvsp[-2].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-2].node)); 
        if ((yyvsp[0].statement) && !(yyvsp[0].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 1956 "tools-temp/parser.yy.c"
    break;

  case 34:
#line 404 "src/executors/parser.y"
      { 
        std::cout << "Building If-Else: " << ((yyvsp[-8].node) ? (yyvsp[-8].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "IfElse"; 
        (yyval.statement)->value = (yyvsp[-8].node) ? (yyvsp[-8].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-8].node)); 
        if ((yyvsp[-5].statement) && !(yyvsp[-5].statement)->statements.empty()) {
            (yyval.statement)->statements.insert((yyval.statement)->statements.end(), (yyvsp[-5].statement)->statements.begin(), (yyvsp[-5].statement)->statements.end());
        }
        if ((yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty()) {
            (yyval.statement)->statements.insert((yyval.statement)->statements.end(), (yyvsp[-1].statement)->statements.begin(), (yyvsp[-1].statement)->statements.end());
        }
      }
#line 1974 "tools-temp/parser.yy.c"
    break;

  case 35:
#line 418 "src/executors/parser.y"
      { 
        std::cout << "Building If-Else: " << ((yyvsp[-4].node) ? (yyvsp[-4].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "IfElse"; 
        (yyval.statement)->value = (yyvsp[-4].node) ? (yyvsp[-4].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-4].node)); 
        if ((yyvsp[-2].statement) && !(yyvsp[-2].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[-2].statement));
        } else {
            delete (yyvsp[-2].statement);
        }
        if ((yyvsp[0].statement) && !(yyvsp[0].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 1996 "tools-temp/parser.yy.c"
    break;

  case 36:
#line 439 "src/executors/parser.y"
      { 
        std::cout << "Building For\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "For"; 
        (yyval.statement)->value = ((yyvsp[-6].node) ? (yyvsp[-6].node)->value : "unknown");
        (yyval.statement)->children.push_back(new ASTNode("Init", (yyvsp[-7].statement)->value)); 
        (yyval.statement)->children.push_back((yyvsp[-6].node)); 
        (yyval.statement)->children.push_back((yyvsp[-4].node)); 
        if ((yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty()) {
            (yyval.statement)->statements = (yyvsp[-1].statement)->statements;
        }
        delete (yyvsp[-7].statement); 
      }
#line 2014 "tools-temp/parser.yy.c"
    break;

  case 37:
#line 453 "src/executors/parser.y"
      { 
        std::cout << "Building For\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "For"; 
        (yyval.statement)->value = ((yyvsp[-4].node) ? (yyvsp[-4].node)->value : "unknown");
        (yyval.statement)->children.push_back(new ASTNode("Init", (yyvsp[-5].statement)->value)); 
        (yyval.statement)->children.push_back((yyvsp[-4].node)); 
        (yyval.statement)->children.push_back((yyvsp[-2].node)); 
        if ((yyvsp[0].statement) && !(yyvsp[0].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
        delete (yyvsp[-5].statement); 
      }
#line 2034 "tools-temp/parser.yy.c"
    break;

  case 38:
#line 472 "src/executors/parser.y"
      { (yyval.node) = (yyvsp[0].node); }
#line 2040 "tools-temp/parser.yy.c"
    break;

  case 39:
#line 474 "src/executors/parser.y"
      { 
        std::cout << "Building Increment: " << ((yyvsp[-1].str) ? *(yyvsp[-1].str) : "null") << "\n"; // Debug
        (yyval.node) = new ASTNode("Increment", ((yyvsp[-1].str) ? *(yyvsp[-1].str) : "unknown") + "++");
        (yyval.node)->children.push_back(new ASTNode("Identifier", (yyvsp[-1].str) ? *(yyvsp[-1].str) : "unknown"));
        delete (yyvsp[-1].str); 
      }
#line 2051 "tools-temp/parser.yy.c"
    break;

  case 40:
#line 481 "src/executors/parser.y"
      { 
        std::cout << "Building PreIncrement: " << ((yyvsp[0].str) ? *(yyvsp[0].str) : "null") << "\n"; // Debug
        (yyval.node) = new ASTNode("PreIncrement", "++" + ((yyvsp[0].str) ? *(yyvsp[0].str) : "unknown"));
        (yyval.node)->children.push_back(new ASTNode("Identifier", (yyvsp[0].str) ? *(yyvsp[0].str) : "unknown"));
        delete (yyvsp[0].str); 
      }
#line 2062 "tools-temp/parser.yy.c"
    break;

  case 41:
#line 491 "src/executors/parser.y"
      { 
        std::cout << "Building While: " << ((yyvsp[-4].node) ? (yyvsp[-4].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "While"; 
        (yyval.statement)->value = (yyvsp[-4].node) ? (yyvsp[-4].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-4].node)); 
        if ((yyvsp[-1].statement) && !(yyvsp[-1].statement)->statements.empty()) {
            (yyval.statement)->statements = (yyvsp[-1].statement)->statements;
        }
      }
#line 2077 "tools-temp/parser.yy.c"
    break;

  case 42:
#line 502 "src/executors/parser.y"
      { 
        std::cout << "Building While: " << ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "While"; 
        (yyval.statement)->value = (yyvsp[-2].node) ? (yyvsp[-2].node)->value : "unknown";
        (yyval.statement)->children.push_back((yyvsp[-2].node)); 
        if ((yyvsp[0].statement) && !(yyvsp[0].statement)->value.empty()) {
            (yyval.statement)->statements.push_back((yyvsp[0].statement));
        } else {
            delete (yyvsp[0].statement);
        }
      }
#line 2094 "tools-temp/parser.yy.c"
    break;

  case 43:
#line 518 "src/executors/parser.y"
      { 
        std::cout << "Building Struct: " << ((yyvsp[-4].str) ? *(yyvsp[-4].str) : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Struct"; 
        std::string id = ((yyvsp[-4].str) && !(yyvsp[-4].str)->empty()) ? *(yyvsp[-4].str) : "unknown";
        (yyval.statement)->value = "struct " + id;
        if ((yyvsp[-2].decl_list) && !(yyvsp[-2].decl_list)->empty()) {
            for (const auto* decl : *(yyvsp[-2].decl_list)) {
                if (decl && !decl->value.empty()) {
                    (yyval.statement)->children.push_back(new ASTNode(decl->type, decl->value));
                }
            }
        }
        delete (yyvsp[-4].str); 
        delete (yyvsp[-2].decl_list); // Delete the vector
      }
#line 2115 "tools-temp/parser.yy.c"
    break;

  case 44:
#line 538 "src/executors/parser.y"
      { 
        std::cout << "Building Assignment: " << ((yyvsp[-3].str) ? *(yyvsp[-3].str) : "null") << ", " << ((yyvsp[-1].node) ? (yyvsp[-1].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Assignment"; 
        std::string id = ((yyvsp[-3].str) && !(yyvsp[-3].str)->empty()) ? *(yyvsp[-3].str) : "unknown";
        std::string val = ((yyvsp[-1].node) && !(yyvsp[-1].node)->value.empty()) ? (yyvsp[-1].node)->value : "0";
        (yyval.statement)->value = id + " = " + val;
        (yyval.statement)->children.push_back((yyvsp[-1].node)); 
        delete (yyvsp[-3].str); 
      }
#line 2130 "tools-temp/parser.yy.c"
    break;

  case 45:
#line 549 "src/executors/parser.y"
      { 
        std::cout << "Building Assignment: " << ((yyvsp[-3].str) ? *(yyvsp[-3].str) : "null") << ", " << ((yyvsp[-1].node) ? (yyvsp[-1].node)->value : "null") << "\n"; // Debug
        (yyval.statement) = new StatementNode(); 
        (yyval.statement)->type = "Assignment"; 
        std::string id = ((yyvsp[-3].str) && !(yyvsp[-3].str)->empty()) ? *(yyvsp[-3].str) : "unknown";
        std::string val = ((yyvsp[-1].node) && !(yyvsp[-1].node)->value.empty()) ? (yyvsp[-1].node)->value : "0";
        (yyval.statement)->value = id + " *= " + val;
        (yyval.statement)->children.push_back((yyvsp[-1].node)); 
        delete (yyvsp[-3].str); 
      }
#line 2145 "tools-temp/parser.yy.c"
    break;

  case 46:
#line 562 "src/executors/parser.y"
                  { (yyval.expr_list) = new std::vector<ASTNode*>(); }
#line 2151 "tools-temp/parser.yy.c"
    break;

  case 47:
#line 564 "src/executors/parser.y"
      { 
        (yyval.expr_list) = new std::vector<ASTNode*>();
        if ((yyvsp[0].node) && !(yyvsp[0].node)->value.empty()) {
            (yyval.expr_list)->push_back((yyvsp[0].node));
        } else {
            delete (yyvsp[0].node);
        }
      }
#line 2164 "tools-temp/parser.yy.c"
    break;

  case 48:
#line 573 "src/executors/parser.y"
      { 
        (yyval.expr_list) = (yyvsp[-2].expr_list) ? (yyvsp[-2].expr_list) : new std::vector<ASTNode*>();
        if ((yyvsp[0].node) && !(yyvsp[0].node)->value.empty()) {
            (yyval.expr_list)->push_back((yyvsp[0].node));
        } else {
            delete (yyvsp[0].node);
        }
      }
#line 2177 "tools-temp/parser.yy.c"
    break;

  case 49:
#line 584 "src/executors/parser.y"
           { (yyval.node) = (yyvsp[0].node); }
#line 2183 "tools-temp/parser.yy.c"
    break;

  case 50:
#line 586 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Add", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " + " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2193 "tools-temp/parser.yy.c"
    break;

  case 51:
#line 592 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Subtract", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " - " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2203 "tools-temp/parser.yy.c"
    break;

  case 52:
#line 598 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Multiply", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " * " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2213 "tools-temp/parser.yy.c"
    break;

  case 53:
#line 604 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Divide", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " / " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2223 "tools-temp/parser.yy.c"
    break;

  case 54:
#line 610 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Modulo", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " % " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2233 "tools-temp/parser.yy.c"
    break;

  case 55:
#line 616 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Greater", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " > " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2243 "tools-temp/parser.yy.c"
    break;

  case 56:
#line 622 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Less", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " < " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2253 "tools-temp/parser.yy.c"
    break;

  case 57:
#line 628 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("LessEqual", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " <= " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2263 "tools-temp/parser.yy.c"
    break;

  case 58:
#line 634 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Equal", ((yyvsp[-2].node) ? (yyvsp[-2].node)->value : "0") + " == " + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "0")); 
        (yyval.node)->children.push_back((yyvsp[-2].node));
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2273 "tools-temp/parser.yy.c"
    break;

  case 59:
#line 640 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Address", "&" + ((yyvsp[0].node) ? (yyvsp[0].node)->value : "unknown")); 
        (yyval.node)->children.push_back((yyvsp[0].node));
      }
#line 2282 "tools-temp/parser.yy.c"
    break;

  case 60:
#line 648 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Identifier", (yyvsp[0].str) ? *(yyvsp[0].str) : "unknown");
        delete (yyvsp[0].str);
      }
#line 2291 "tools-temp/parser.yy.c"
    break;

  case 61:
#line 653 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Number", (yyvsp[0].str) ? *(yyvsp[0].str) : "0");
        delete (yyvsp[0].str);
      }
#line 2300 "tools-temp/parser.yy.c"
    break;

  case 62:
#line 658 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("String", (yyvsp[0].str) ? *(yyvsp[0].str) : "\"\"");
        delete (yyvsp[0].str);
      }
#line 2309 "tools-temp/parser.yy.c"
    break;

  case 63:
#line 662 "src/executors/parser.y"
                               { (yyval.node) = (yyvsp[-1].node); }
#line 2315 "tools-temp/parser.yy.c"
    break;

  case 64:
#line 664 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("Increment", ((yyvsp[-1].str) ? *(yyvsp[-1].str) : "unknown") + "++");
        (yyval.node)->children.push_back(new ASTNode("Identifier", (yyvsp[-1].str) ? *(yyvsp[-1].str) : "unknown"));
        delete (yyvsp[-1].str);
      }
#line 2325 "tools-temp/parser.yy.c"
    break;

  case 65:
#line 670 "src/executors/parser.y"
      { 
        (yyval.node) = new ASTNode("PreIncrement", "++" + ((yyvsp[0].str) ? *(yyvsp[0].str) : "unknown"));
        (yyval.node)->children.push_back(new ASTNode("Identifier", (yyvsp[0].str) ? *(yyvsp[0].str) : "unknown"));
        delete (yyvsp[0].str);
      }
#line 2335 "tools-temp/parser.yy.c"
    break;


#line 2339 "tools-temp/parser.yy.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 676 "src/executors/parser.y"

