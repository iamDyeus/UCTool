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
    std::string currentFunctionReturnType;
    std::map<std::string, std::vector<std::vector<std::string>>> functionSignatures;
    std::map<std::string, std::string> variableInitialValues;
    std::vector<std::string> loopLabels;

    std::string newTemp();
    std::string newLabel();
    std::string allocateRegister();
    void freeRegister(const std::string& reg);
    std::shared_ptr<DAGNode> findDAGNode(const std::string& op, const std::string& arg1, const std::string& arg2);
    std::shared_ptr<DAGNode> createDAGNode(const std::string& op, const std::string& value,
                                           const std::vector<std::string>& args, int line);
    void analyzeNode(const std::shared_ptr<ASTNode>& node);
    void analyzeFunction(const std::shared_ptr<ASTNode>& node);
    void analyzeForLoop(const std::shared_ptr<ASTNode>& node);
    void analyzeWhileLoop(const std::shared_ptr<ASTNode>& node);
    void analyzeIfElse(const std::shared_ptr<ASTNode>& node);
    void analyzeVarDecl(const std::shared_ptr<ASTNode>& node);
    void analyzeAssignment(const std::shared_ptr<ASTNode>& node);
    void analyzeCompoundAssign(const std::shared_ptr<ASTNode>& node);
    void analyzeFunctionCall(const std::shared_ptr<ASTNode>& node);
    void analyzeReturn(const std::shared_ptr<ASTNode>& node);
    std::string getExpressionType(const std::shared_ptr<ASTNode>& node);
    bool isCompatibleType(const std::string& type1, const std::string& type2);
    bool validateBinaryOperation(const std::string& op, const std::string& type1, const std::string& type2);
    std::string generateExpressionTAC(const std::shared_ptr<ASTNode>& node);
    void generateTAC(const std::shared_ptr<ASTNode>& node);
    void generateForLoopTAC(const std::shared_ptr<ASTNode>& node);
    void generateIfElseTAC(const std::shared_ptr<ASTNode>& node);
    void generateFunctionTAC(const std::shared_ptr<ASTNode>& node);
    void generateCompoundAssignTAC(const std::shared_ptr<ASTNode>& node);
    void printTAC() const;
    void generateTargetCode();
    void generateFunctionPrologue(const std::string& funcName);
    void generateFunctionEpilogue();
    void generateLoopCode(const std::shared_ptr<ASTNode>& node);
    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const;

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a);
    void analyzeSemantics();
    void generateTACOnly();
    void generateTargetCodeOnly();
    void saveASTToFile(const std::string& filename) const;
    const std::vector<SemanticIssue>& getIssues() const;
};

#endif