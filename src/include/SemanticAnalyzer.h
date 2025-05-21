#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include "SymbolTable.h"
#include "TAC.h"
#include "DAG.h"
#include "AST.h"

class SemanticAnalyzer {
private:
    std::shared_ptr<ASTNode> ast;
    SymbolTable symbolTable;
    std::vector<TACInstruction> tacInstructions;
    std::vector<std::shared_ptr<DAGNode>> dagNodes;
    std::vector<std::string> registers;
    std::vector<SemanticIssue> issues;
    int tempCounter;
    int labelCounter;
    int dagNodeCounter;
    size_t registerCounter;
    std::map<std::string, std::vector<std::vector<std::string>>> functionSignatures;

    std::string newTemp();
    std::string newLabel();
    std::string allocateRegister();
    void freeRegister(const std::string& reg);
    std::shared_ptr<DAGNode> findDAGNode(const std::string& op, const std::string& arg1, const std::string& arg2);
    std::shared_ptr<DAGNode> createDAGNode(const std::string& op, const std::string& value,
                                           const std::vector<std::string>& args, int line);
    void analyzeNode(const std::shared_ptr<ASTNode>& node);
    std::string getExpressionType(const std::shared_ptr<ASTNode>& node);
    std::string generateExpressionTAC(const std::shared_ptr<ASTNode>& node);
    void generateTAC(const std::shared_ptr<ASTNode>& node);
    void printTAC() const;
    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const;

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a);
    void analyzeSemantics();
    void generateTACOnly();
    void saveASTToFile(const std::string& filename) const;
    const std::vector<SemanticIssue>& getIssues() const;
};

#endif