#include "Parser.h"
#include "SemanticAnalyzer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ast_file>.ast" << std::endl;
        return 1;
    }

    try {
        // Parse the AST file
        auto ast = readASTFromFile(argv[1]);
        
        // Perform semantic analysis and TAC generation
        SemanticAnalyzer analyzer(ast);
        analyzer.analyze();
        
        // Save the processed AST to a file
        analyzer.saveASTToFile("processed_ast.txt");
        
        // Output results
        const auto& issues = analyzer.getIssues();
        if (!issues.empty()) {
            std::cout << "\nSemantic Analysis Results:\n";
            for (const auto& issue : issues) {
                std::cout << issue.type << ": " << issue.description << " " << issue.status << "\n";
            }
        } else {
            std::cout << "\nNo semantic errors or warnings found.\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}