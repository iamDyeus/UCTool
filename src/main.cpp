#include "../include/semantic_analyzer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ast_file>.ast" << std::endl;
        return 1;
    }

    try {
        auto ast = readASTFromFile(argv[1]);
        SemanticAnalyzer analyzer(ast);
        auto errors = analyzer.analyze();
        for (const auto& error : errors) {
            std::cout << error << std::endl;
        }
        if (errors.empty()) {
            std::cout << "Semantic analysis passed." << std::endl;
            analyzer.saveASTToFile("output2.ast");
            std::cout << "Annotated AST saved to output2.ast" << std::endl;
        } else {
            std::cout << "Semantic analysis failed. No output file generated." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}