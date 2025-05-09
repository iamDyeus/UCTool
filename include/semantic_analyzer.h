#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

// Symbol table entry
struct Symbol {
    std::string type;
    bool initialized;
    Symbol();
    Symbol(const std::string& t);
};

// Symbol table with scope management
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<std::string> errors;
    std::unordered_map<std::string, std::string> macros; // Store preprocessor macros
    std::unordered_map<std::string, bool> structs; // Store struct definitions

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, const std::string& type, int line);
    const Symbol* lookup(const std::string& name, int line) const;
    bool defineMacro(const std::string& name, const std::string& value, int line);
    bool defineStruct(const std::string& name, int line);
    const std::vector<std::string>& getErrors() const;
};

// AST node types
enum class NodeType {
    Program, Function, LocalDeclaration, Call, IfElse, Return,
    Identifier, Number, String, Address, Modulo, Equal,
    Preprocessor, Struct, Assignment, Add, While, Less, Increment, Expression
};

// Forward declaration for ASTNode
struct ASTNode;

// Semantic analyzer
class SemanticAnalyzer {
private:
    std::shared_ptr<ASTNode> ast;
    SymbolTable symbolTable;
    mutable std::vector<std::string> errors; // Mutable to allow error collection in const methods
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> functionSignatures;

    void analyzeNode(const std::shared_ptr<ASTNode>& node);
    std::string getExpressionType(const std::shared_ptr<ASTNode>& node) const;
    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const;

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a);
    std::vector<std::string> analyze();
    void saveASTToFile(const std::string& filename) const;
    ~SemanticAnalyzer();
};

// AST node structure
struct ASTNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(NodeType t, std::string val = "", std::string th = "", int l = 1);
};

// Parse a single line into node type, value, and typeHint
struct ParsedNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    std::vector<std::shared_ptr<ASTNode>> children; // Added to support child nodes
};

ParsedNode parseLine(const std::string& line);
std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename);

#endif // SEMANTIC_ANALYZER_H