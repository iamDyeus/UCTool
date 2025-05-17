#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
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
        std::cerr << "Could not open ../temp/lex-tokens.txt for reading\n";
        return;
    }

    std::string line;
    std::string section;
    while (std::getline(infile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        if (line.empty()) continue;

        if (line == "[Macros]") {
            section = "Macros";
            continue;
        } else if (line == "[Included Files]") {
            section = "Included Files";
            continue;
        } else if (line == "[Recognized Tokens]") {
            section = "Recognized Tokens";
            continue;
        } else if (line == "[Unknown Tokens]") {
            section = "Unknown Tokens";
            continue;
        }

        if (section == "Recognized Tokens") {
            // Format: type|value|line_no|col_no
            std::stringstream ss(line);
            std::string type, value, line_no, col_no;
            std::getline(ss, type, '|');
            std::getline(ss, value, '|');
            std::getline(ss, line_no, '|');
            std::getline(ss, col_no, '|');

            // Unescape | in value
            std::string unescaped_value;
            bool escape = false;
            for (char c : value) {
                if (escape) {
                    unescaped_value += c;
                    escape = false;
                } else if (c == '\\') {
                    escape = true;
                } else {
                    unescaped_value += c;
                }
            }

            try {
                tokens.emplace_back(Tokens{type, unescaped_value, std::stoi(line_no), std::stoi(col_no)});
            } catch (const std::exception& e) {
                std::cerr << "Error parsing token: " << line << "\n";
            }
        } else if (section == "Unknown Tokens") {
            // Format: value|line_no|col_no
            std::stringstream ss(line);
            std::string value, line_no, col_no;
            std::getline(ss, value, '|');
            std::getline(ss, line_no, '|');
            std::getline(ss, col_no, '|');

            // Unescape | in value
            std::string unescaped_value;
            bool escape = false;
            for (char c : value) {
                if (escape) {
                    unescaped_value += c;
                    escape = false;
                } else if (c == '\\') {
                    escape = true;
                } else {
                    unescaped_value += c;
                }
            }

            try {
                unknown_tokens.emplace_back(UnknownTokens{unescaped_value, std::stoi(line_no), std::stoi(col_no)});
            } catch (const std::exception& e) {
                std::cerr << "Error parsing unknown token: " << line << "\n";
            }
        }
    }
    infile.close();

    // Debug: Print loaded tokens
    std::cout << "Loaded Recognized Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "Type: " << token.type << ", Value: " << token.value << ", Line: " << token.line_no << ", Col: " << token.col_no << "\n";
    }
    std::cout << "Loaded Unknown Tokens:\n";
    for (const auto& token : unknown_tokens) {
        std::cout << "Value: " << token.value << ", Line: " << token.line_no << ", Col: " << token.col_no << "\n";
    }

    // Run parser
    token_iterator = new TokenIterator(tokens, unknown_tokens);
    //yydebug = 1; // Enable Bison debug
    if (yyparse() == 0 && parse_result != nullptr) {
        std::ofstream outfile("../temp/parser-output.ast");
        if (!outfile.is_open()) {
            std::cerr << "Could not open ../temp/parser-output.ast for writing\n";
            return;
        }
        outfile << parse_result->to_string();
        outfile.close();
        std::cout << "\nParse Tree:\n" << parse_result->to_string() << "\n";
    } else {
        std::cerr << "Parsing failed\n";
    }
    delete token_iterator;
    delete parse_result;
    token_iterator = nullptr;
    parse_result = nullptr;
}
