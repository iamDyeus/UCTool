#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <tuple>

enum class NodeType {
    Program,
    Function,
    LocalDeclaration,
    Call,
    IfElse,
    Return,
    Identifier,
    Number,
    String,
    Address,
    Modulo,
    Equal,
    Preprocessor,
    Declaration
};

class Symbol {
public:
    Symbol();
    Symbol(const std::string& t);
    std::string type;
    bool initialized;
};

class SymbolTable {
private:
    std::vector<std::map<std::string, Symbol>> scopes;
    std::vector<std::string> errors;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    bool declare(const std::string& name, const std::string& type, int line);
    const Symbol* lookup(const std::string& name, int line) const;
    const std::vector<std::string>& getErrors() const;
    size_t getScopeCount() const;
    void printSymbolTable() const;
    void printScopeResolutionTable() const;
};

class ASTNode {
public:
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(NodeType t, std::string val, std::string th, int l);
};

class SemanticAnalyzer {
private:
    std::shared_ptr<ASTNode> ast;
    SymbolTable symbolTable;
    mutable std::vector<std::string> errors; // Made mutable
    std::map<std::string, std::vector<std::vector<std::string>>> functionSignatures;
    mutable std::vector<std::tuple<int, std::string, std::string, std::string>> typeCheckingTable;

    void analyzeNode(const std::shared_ptr<ASTNode>& node);
    std::string getExpressionType(const std::shared_ptr<ASTNode>& node) const;
    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const;
    void printTypeCheckingTable() const;

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a);
    ~SemanticAnalyzer();
    std::vector<std::string> analyze();
    void saveASTToFile(const std::string& filename) const;
};

std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename);

#endif