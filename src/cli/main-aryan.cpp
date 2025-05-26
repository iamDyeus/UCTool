#include <iostream>
#include <string>
#include <cstring>
#include <filesystem>

// Forward declarations
extern void runSemanticAnalysis(const char* filename);
extern void runTACGeneration(const char* filename);
extern void runTargetCodeGeneration(const char* filename);

// Function to check if file exists
bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [--semantic] [--intermediate] [--target]\n";
        return 1;
    }

    std::string source_file;
    bool semantic_mode = false;
    bool intermediate_mode = false;
    bool target_mode = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--semantic") == 0) {
            semantic_mode = true;
        } else if (std::strcmp(argv[i], "--intermediate") == 0) {
            intermediate_mode = true;
        } else if (std::strcmp(argv[i], "--target") == 0) {
            target_mode = true;
        } else {
            source_file = argv[i];
        }
    }

    if (!semantic_mode && !intermediate_mode && !target_mode) {
        std::cerr << "Error: At least one of --semantic, --intermediate, or --target is required\n";
        std::cerr << "Usage: " << argv[0] << " <source_file> [--semantic] [--intermediate] [--target]\n";
        return 1;
    }

    if (source_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: " << argv[0] << " <source_file> [--semantic] [--intermediate] [--target]\n";
        return 1;
    }

    // The AST file that we'll actually use
    const std::string ast_file = "temp/parser-output.ast";

    // Check if AST file exists
    if (!fileExists(ast_file)) {
        std::cerr << "Error: AST file not found. Please make sure to parse the source file first.\n";
        return 1;
    }

    // Run the requested analyses on the AST file
    if (semantic_mode) {
        std::cout << "Running semantic analysis on " << source_file << "...\n";
        runSemanticAnalysis(ast_file.c_str());
    }
    if (intermediate_mode) {
        std::cout << "Generating intermediate code for " << source_file << "...\n";
        runTACGeneration(ast_file.c_str());
    }
    if (target_mode) {
        std::cout << "Generating target code for " << source_file << "...\n";
        runTargetCodeGeneration(ast_file.c_str());
    }

    return 0;
}