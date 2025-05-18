#include "../include/Parser.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/SymbolTable.h"
#include <iostream>

void runTACGeneration(const char* filename) {
    try {
        std::shared_ptr<ASTNode> ast = readASTFromFile(filename);
        SemanticAnalyzer analyzer(ast);
        analyzer.generateTACOnly();
        const auto& issues = analyzer.getIssues();
        if (!issues.empty()) {
            std::cerr << "Semantic issues found during TAC generation:\n";
            for (const auto& issue : issues) {
                std::cerr << issue.type << ": " << issue.description << " " << issue.status << "\n";
            }
        } else {
            std::cout << "TAC generation completed successfully.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during TAC generation: " << e.what() << "\n";
        exit(1);
    }
}