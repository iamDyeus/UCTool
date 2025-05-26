#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include "../include/lexer_utils.hpp"
#include "../include/lexer.h"

// Define line_num
int line_num = 1;

void performLexicalAnalysis(const char* filename) {    
    // Ensure ../temp/ directory exists
    std::filesystem::create_directories("../temp");
    
    yyin = fopen(filename, "r");
    if (!yyin) {
        std::cerr << "Error: Could not open input file: " << filename << "\n";
        return;
    }

    while (yylex() != 0) {} // Loop until EOF
    fclose(yyin);

    // Check for unknown tokens
    if (!unknown_tokens.empty()) {
        for (const auto& token : unknown_tokens) {
            std::cerr << "Error: Unknown tokens detected.";
        }        
        return;
    }

    std::ofstream outfile("../temp/lex-tokens.txt");
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open temp/lex-tokens.txt for writing\n";
        return;
    }

    // Write enhanced tabular format
    outfile << "+----------------------+----------------------------------------+--------+--------+\n";
    outfile << "| Token Type           | Value                                  | Line   | Col    |\n";
    outfile << "+----------------------+----------------------------------------+--------+--------+\n";
    for (const auto& token : tokens) {
        std::string escaped_value = token.value;
        // Escape special characters
        size_t pos = 0;
        while ((pos = escaped_value.find("\t", pos)) != std::string::npos) {
            escaped_value.replace(pos, 1, "\\t");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped_value.find("\n", pos)) != std::string::npos) {
            escaped_value.replace(pos, 1, "\\n");
            pos += 2;
        }
        // Truncate value if too long
        if (escaped_value.length() > 36) {
            escaped_value = escaped_value.substr(0, 33) + "...";
        }
        outfile << "| " << std::left << std::setw(20) << token.type 
                << " | " << std::left << std::setw(38) << escaped_value 
                << " | " << std::right << std::setw(6) << token.line_no 
                << " | " << std::right << std::setw(6) << token.col_no 
                << " |\n";
    }
    outfile << "+----------------------+----------------------------------------+--------+--------+\n";
    outfile.close();

    // Print only tabular output to terminal
    std::cout << "+----------------------+----------------------------------------+--------+--------+\n";
    std::cout << "| Token Type           | Value                                  | Line   | Col    |\n";
    std::cout << "+----------------------+----------------------------------------+--------+--------+\n";
    for (const auto& token : tokens) {
        std::string display_value = token.value;
        // Replace newlines with descriptive text for display
        size_t pos = 0;
        while ((pos = display_value.find("\n", pos)) != std::string::npos) {
            display_value.replace(pos, 1, "(newline)");
            pos += 9;
        }
        // Truncate value for display
        if (display_value.length() > 36) {
            display_value = display_value.substr(0, 33) + "...";
        }
        std::cout << "| " << std::left << std::setw(20) << token.type 
                  << " | " << std::left << std::setw(38) << display_value 
                  << " | " << std::right << std::setw(6) << token.line_no 
                  << " | " << std::right << std::setw(6) << token.col_no 
                  << " |\n";
    }
    std::cout << "+----------------------+----------------------------------------+--------+--------+\n";
}