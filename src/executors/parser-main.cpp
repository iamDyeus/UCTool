#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "../include/lexer_utils.hpp"
#include "../include/parser_utils.hpp"
#include "../include/lexer.h"


ProgramNode* parse_result = nullptr;
TokenIterator* token_iterator = nullptr;

void performParsing() {
    // Clear existing tokens
    tokens.clear();
    unknown_tokens.clear();

    // Read lex-tokens.txt
    std::ifstream infile("../temp/lex-tokens.txt");
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open ../temp/lex-tokens.txt for reading\n";
        return;
    }

    std::string line;
    bool header_processed = false;
    while (std::getline(infile, line)) {
        // Remove trailing \r (for Windows compatibility)
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        if (line.empty()) continue;

        // Skip header and border lines
        if (!header_processed) {
            if (line.find("Token Type           | Value") != std::string::npos ||
                line.find("+----------------------+----------------------------------------+--------+--------+") != std::string::npos) {
                continue;
            }
            header_processed = true;
        }

        // Parse tabular line: | Token Type | Value | Line | Col |
        // Remove leading/trailing | and split
        if (line.front() == '|' && line.back() == '|') {
            line = line.substr(1, line.length() - 2);
        }
        std::stringstream ss(line);
        std::string type, value, line_no, col_no;
        std::getline(ss, type, '|');
        std::getline(ss, value, '|');
        std::getline(ss, line_no, '|');
        std::getline(ss, col_no, '|');

        // Trim whitespace
        type.erase(0, type.find_first_not_of(" \t"));
        type.erase(type.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        line_no.erase(0, line_no.find_first_not_of(" \t"));
        line_no.erase(line_no.find_last_not_of(" \t") + 1);
        col_no.erase(0, col_no.find_first_not_of(" \t"));
        col_no.erase(col_no.find_last_not_of(" \t") + 1);

        // Unescape tabs and newlines in value
        std::string unescaped_value;
        bool escape = false;
        for (char c : value) {
            if (escape) {
                if (c == 't') unescaped_value += '\t';
                else if (c == 'n') unescaped_value += '\n';
                else unescaped_value += c;
                escape = false;
            } else if (c == '\\') {
                escape = true;
            } else {
                unescaped_value += c;
            }
        }

        try {
            if (type == "Unknown") {
                unknown_tokens.emplace_back(UnknownTokens{unescaped_value, std::stoi(line_no), std::stoi(col_no)});
            } else {
                tokens.emplace_back(Tokens{type, unescaped_value, std::stoi(line_no), std::stoi(col_no)});
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing token: " << line << "\n";
        }
    }
    infile.close();

    // Check for parsing errors
    if (tokens.empty() && unknown_tokens.empty()) {
        std::cerr << "Error: No valid tokens found.\n";
        return;
    }

    // Run parser
    token_iterator = new TokenIterator(tokens, unknown_tokens);
    if (yyparse() == 0 && parse_result != nullptr) {
        std::ofstream outfile("../temp/parser-output.ast");
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not open ../temp/parser-output.ast for writing\n";
            return;
        }
        outfile << parse_result->to_string();
        outfile.close();
        std::cout << "\nParse Tree:\n" << parse_result->to_string() << "\n";
    } else {
        std::cerr << "Error: Parsing failed.";
    }
    delete token_iterator;
    delete parse_result;
    token_iterator = nullptr;
    parse_result = nullptr;
}