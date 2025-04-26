#include <iostream>
#include <fstream>
#include <filesystem>
#include "../include/lexer_utils.hpp"
#include "../include/lexer.h"

// Define line_num
int line_num = 1;

void performLexicalAnalysis(const char* filename) {
    // Ensure ../temp/ directory exists
    std::filesystem::create_directories("../temp");
    
    // Debug: Print current working directory
    std::cout << "Current working directory: " << std::filesystem::current_path() << "\n";
    
    // Debug: Verify file path
    std::string outfile_path = "../temp/lex-tokens.txt";
    std::cout << "Attempting to write to: " << std::filesystem::absolute(outfile_path) << "\n";
    
    yyin = fopen(filename, "r");
    if (!yyin) {
        std::cerr << "Could not open input file: " << filename << "\n";
        return;
    }

    while (yylex() != 0) {} // Loop until EOF
    fclose(yyin);

    std::ofstream outfile(outfile_path);
    if (!outfile.is_open()) {
        std::cerr << "Could not open " << outfile_path << " for writing\n";
        return;
    }

    outfile << "[Macros]\n";
    for (const auto& macro : macros) {
        outfile << macro.first << "|" << macro.second << "\n";
    }

    outfile << "[Included Files]\n";
    for (const auto& file : included_files) {
        outfile << file << "\n";
    }

    outfile << "[Recognized Tokens]\n";
    for (const auto& token : tokens) {
        std::string escaped_value = token.value;
        size_t pos = 0;
        while ((pos = escaped_value.find("|", pos)) != std::string::npos) {
            escaped_value.replace(pos, 1, "\\|");
            pos += 2;
        }
        outfile << token.type << "|" << escaped_value << "|" << token.line_no << "|" << token.col_no << "\n";
    }

    outfile << "[Unknown Tokens]\n";
    for (const auto& token : unknown_tokens) {
        std::string escaped_value = token.value;
        size_t pos = 0;
        while ((pos = escaped_value.find("|", pos)) != std::string::npos) {
            escaped_value.replace(pos, 1, "\\|");
            pos += 2;
        }
        outfile << escaped_value << "|" << token.line_no << "|" << token.col_no << "\n";
    }

    outfile.close();

    std::cout << "\nRecognized Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "Debug: Printing token: Type=" << token.type << ", Value=" << token.value << ", Line=" << token.line_no << ", Col=" << token.col_no << "\n";
        std::cout << "Type: " << token.type << ", Value: " << token.value << ", Line: " << token.line_no << ", Col: " << token.col_no << "\n";
    }
    std::cout << "\nUnknown Tokens:\n";
    for (const auto& token : unknown_tokens) {
        std::cout << "Debug: Printing unknown token: Value=" << token.value << ", Line=" << token.line_no << ", Col=" << token.col_no << "\n";
        std::cout << "Value: " << token.value << ", Line: " << token.line_no << ", Col: " << token.col_no << "\n";
    }
}