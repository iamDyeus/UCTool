#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

// Symbol table entry
struct Symbol {
    std::string type;
    std::string scope;
    std::string attributes;
    bool initialized;
    bool used;
    Symbol();
    Symbol(const std::string& t, const std::string& s, const std::string& a);
};

// Type checking entry
struct TypeCheck {
    std::string location;
    std::string description;
    std::string status;
    TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat);
};

// Scope checking entry
struct ScopeCheck {
    std::string scope;
    std::string action;
    int symbolCount;
    ScopeCheck(const std::string& s, const std::string& a, int count);
};

// Semantic error/warning entry
struct SemanticIssue {
    std::string type;
    std::string description;
    std::string status;
    SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat);
};

// Symbol table with scope management
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<std::string> scopeNames;
    std::vector<SemanticIssue> issues;
    std::unordered_map<std::string, Symbol> macros;
    std::unordered_map<std::string, Symbol> structs;
    std::unordered_map<std::string, Symbol> functions;
    mutable std::vector<TypeCheck> typeChecks;
    mutable std::vector<ScopeCheck> scopeChecks;
    bool hasStdioInclude;

public:
    SymbolTable();
    void enterScope(const std::string& scopeName);
    void exitScope();
    bool declare(const std::string& name, const std::string& type, const std::string& attributes, int line);
    bool defineMacro(const std::string& name, const std::string& value, int line);
    bool defineStruct(const std::string& name, int line);
    bool defineFunction(const std::string& name, const std::string& type, int line);
    const Symbol* lookup(const std::string& name, int line) const;
    void markUsed(const std::string& name);
    void setStdioInclude();
    bool hasStdio() const;
    void addTypeCheck(const std::string& location, const std::string& description, const std::string& status) const;
    void addWarning(const std::string& description, int line);
    void checkUnusedSymbols();
    void printSymbolTable() const;
    void printTypeChecks() const;
    void printScopeChecks() const;
    void printIssues() const;
    const std::vector<SemanticIssue>& getIssues() const;
    bool hasOnlyWarnings() const;
    std::string getCurrentScope() const; // Added declaration
};

// AST node types
enum class NodeType {
    Program, Function, LocalDeclaration, Call, IfElse, Return,
    Identifier, Number, String, Address, Modulo, Equal,
    Preprocessor, Struct, Assignment, Add, While, Less, Increment, Expression
};

// Map string node types to enum
static const std::unordered_map<std::string, NodeType> nodeTypeMap = {
    {"Program", NodeType::Program},
    {"Function", NodeType::Function},
    {"LocalDeclaration", NodeType::LocalDeclaration},
    {"Call", NodeType::Call},
    {"IfElse", NodeType::IfElse},
    {"Return", NodeType::Return},
    {"Identifier", NodeType::Identifier},
    {"Number", NodeType::Number},
    {"String", NodeType::String},
    {"Address", NodeType::Address},
    {"Modulo", NodeType::Modulo},
    {"Equal", NodeType::Equal},
    {"Preprocessor", NodeType::Preprocessor},
    {"Struct", NodeType::Struct},
    {"Assignment", NodeType::Assignment},
    {"Add", NodeType::Add},
    {"While", NodeType::While},
    {"Less", NodeType::Less},
    {"Increment", NodeType::Increment},
    {"Expression", NodeType::Expression}
};

// AST node structure
struct ASTNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    std::vector<std::shared_ptr<ASTNode>> children;
    mutable std::string cachedType;
    ASTNode(NodeType t, std::string val = "", std::string th = "", int l = 1);
};

// Semantic analyzer
class SemanticAnalyzer {
private:
    std::shared_ptr<ASTNode> ast;
    SymbolTable symbolTable;
    std::vector<SemanticIssue> issues;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> functionSignatures;
    void analyzeNode(const std::shared_ptr<ASTNode>& node);
    std::string getExpressionType(const std::shared_ptr<ASTNode>& node);
    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const;

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a);
    void analyze();
    void saveASTToFile(const std::string& filename) const;
    const std::vector<SemanticIssue>& getIssues() const;
};

// Parsed node structure for parsing AST
struct ParsedNode {
    NodeType type;
    std::string value;
    std::string typeHint;
};

ParsedNode parseLine(const std::string& line);
std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename);

#endif // SEMANTIC_ANALYZER_H