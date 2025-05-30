// Entry point for CLI
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../ai/llm_explainer.h"

// Forward declarations
extern void performLexicalAnalysis(const char* filename);
extern void performParsing();
extern void runSemanticAnalysis(const char* filename);
extern void runTACGeneration(const char* filename);
extern void runTargetCodeGeneration(const char* filename);

// Function to check if file exists
bool fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> [--lexical] [--parse] [--semantic] [--intermediate] [--target] [--help]\n";
        return 1;
    }

    std::string source_file;
    bool lexical_mode = false;
    bool parse_mode = false;
    bool semantic_mode = false;
    bool intermediate_mode = false;
    bool target_mode = false;
    bool help_mode = false;

    // Parse command-line arguments, only allow --help at the end
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--lexical") == 0) {
            lexical_mode = true;
        } else if (std::strcmp(argv[i], "--parse") == 0) {
            parse_mode = true;
        } else if (std::strcmp(argv[i], "--semantic") == 0) {
            semantic_mode = true;
        } else if (std::strcmp(argv[i], "--intermediate") == 0) {
            intermediate_mode = true;
        } else if (std::strcmp(argv[i], "--target") == 0) {
            target_mode = true;
        } else if (std::strcmp(argv[i], "--help") == 0 && i == argc - 1) {
            help_mode = true;
        } else {
            source_file = argv[i];
        }
    }

    // Check if at least one mode is specified
    if (!lexical_mode && !parse_mode && !semantic_mode && !intermediate_mode && !target_mode) {
        std::cerr << "Error: At least one of --lexical, --parse, --semantic, --intermediate, or --target is required\n";
        std::cerr << "Usage: " << argv[0] << " <source_file> [--lexical] [--parse] [--semantic] [--intermediate] [--target] [--help]\n";
        return 1;
    }

    // Check if source file is provided
    if (source_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: " << argv[0] << " <source_file> [--lexical] [--parse] [--semantic] [--intermediate] [--target] [--help]\n";
        return 1;
    }

    // Ensure temp directory exists
    std::filesystem::create_directory("../temp");

    // The AST file used for semantic, intermediate, and target modes
    const std::string ast_file = "../temp/parser-output.ast";

    // Check if AST file exists for semantic, intermediate, or target modes
    if ((semantic_mode || intermediate_mode || target_mode) && !fileExists(ast_file)) {
        std::cerr << "Error: AST file not found at 'temp/parser-output.ast'. Please run with --parse first.\n";
        return 1;
    }

    std::string stage, input_data, output_data;
    if (lexical_mode) {
        performLexicalAnalysis(source_file.c_str());
        stage = "lexical";
        // Read input and output for help
        std::ifstream in_file(source_file);
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/lex-tokens.txt");
        if (!out_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/lex-tokens.txt' for AI explanation\n";
        }
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }
    if (parse_mode) {
        performParsing();
        stage = "parse";
        // Read input and output for help
        std::ifstream in_file("../temp/lex-tokens.txt");
        if (!in_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/lex-tokens.txt' for AI explanation\n";
        }
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/parser-output.ast");
        if (!out_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/parser-output.ast' for AI explanation\n";
        }
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }
    if (semantic_mode) {
        std::cout << "Running semantic analysis on " << source_file << "...\n";
        runSemanticAnalysis(ast_file.c_str());
        stage = "semantic";
        // Read input and output for help
        std::ifstream in_file(ast_file);
        if (!in_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/parser-output.ast' for AI explanation\n";
        }
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/semantic-output.txt");
        if (!out_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/semantic-output.txt' for AI explanation\n";
        }
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }
    if (intermediate_mode) {
        std::cout << "Generating intermediate code for " << source_file << "...\n";
        runTACGeneration(ast_file.c_str());
        stage = "intermediate";
        // Read input and output for help
        std::ifstream in_file(ast_file);
        if (!in_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/parser-output.ast' for AI explanation\n";
        }
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/tac-output.txt");
        if (!out_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/tac-output.txt' for AI explanation\n";
        }
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }
    if (target_mode) {
        std::cout << "Generating target code for " << source_file << "...\n";
        runTargetCodeGeneration(ast_file.c_str());
        stage = "target";
        // Read input and output for help
        std::ifstream in_file(ast_file);
        if (!in_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/parser-output.ast' for AI explanation\n";
        }
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/target-output.txt");
        if (!out_file.is_open()) {
            std::cerr << "Warning: Could not open 'temp/target-output.txt' for AI explanation\n";
        }
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }

    if (help_mode) {
        std::string explanation = generate_ai_help(stage, source_file, input_data, output_data);
        std::cout << "===== AI EXPLANATION =====\n";
        std::cout << explanation << std::endl;
        std::cout << "=========================\n";
    }

    return 0;
}