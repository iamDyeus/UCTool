// Entry point for CLI
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "../ai/llm_explainer.h"

extern void performLexicalAnalysis(const char* filename);
extern void performParsing();

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse] [--help]\n";
        return 1;
    }

    std::string filename;
    bool lexical_mode = false;
    bool parse_mode = false;
    bool help_mode = false;

    // Only allow --help at the end
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--lexical") == 0) {
            lexical_mode = true;
        } else if (std::strcmp(argv[i], "--parse") == 0) {
            parse_mode = true;
        } else if (std::strcmp(argv[i], "--help") == 0 && i == argc - 1) {
            help_mode = true;
        } else {
            filename = argv[i];
        }
    }

    if (!lexical_mode && !parse_mode) {
        std::cerr << "Error: At least one of --lexical or --parse is required\n";
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse] [--help]\n";
        return 1;
    }

    if (filename.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse] [--help]\n";
        return 1;
    }

    std::string stage, input_data, output_data;
    if (lexical_mode) {
        performLexicalAnalysis(filename.c_str());
        stage = "lexical";
        // Read input and output for help
        std::ifstream in_file(filename);
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/lex-tokens.txt");
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }
    if (parse_mode) {
        performParsing();
        stage = "parse";
        // Read input and output for help
        std::ifstream in_file("../temp/lex-tokens.txt");
        std::stringstream in_buf;
        in_buf << in_file.rdbuf();
        input_data = in_buf.str();
        std::ifstream out_file("../temp/parser-output.ast");
        std::stringstream out_buf;
        out_buf << out_file.rdbuf();
        output_data = out_buf.str();
    }

    if (help_mode) {
        std::string explanation = generate_ai_help(stage, filename, input_data, output_data);
        std::cout << "===== AI EXPLANATION =====\n";
        std::cout << explanation << std::endl;
        std::cout << "=========================\n";
    }

    return 0;
}