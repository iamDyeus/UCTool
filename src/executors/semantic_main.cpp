#include "../include/Parser.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/SymbolTable.h"
#include <iostream>

void runSemanticAnalysis(const char* filename) {
    try {
        std::shared_ptr<ASTNode> ast = readASTFromFile(filename);
        SemanticAnalyzer analyzer(ast);
        analyzer.analyzeSemantics();
        const auto& issues = analyzer.getIssues();
        if (!issues.empty()) {
            std::cerr << "Semantic issues found:\n";
            for (const auto& issue : issues) {
                std::cerr << issue.type << ": " << issue.description << " " << issue.status << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during semantic analysis: " << e.what() << "\n";
        exit(1);
    }
}