#include <iostream>
#include "../include/semantic_analyzer.h"

// Read AST from file
std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::vector<std::pair<int, std::string>> lines;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        int indentLevel = 0;
        for (char c : line) {
            if (c == ' ' || c == '\t' || static_cast<unsigned char>(c) == 0xA0) indentLevel++;
            else break;
        }
        lines.emplace_back(indentLevel, line);
    }

    if (lines.empty()) {
        throw std::runtime_error("Empty AST file: " + filename);
    }

    std::vector<std::shared_ptr<ASTNode>> nodeStack;
    std::vector<int> indentStack;
    int lineNumber = 1;

    for (const auto& [indent, lineText] : lines) {
        try {
            ParsedNode parsed = parseLine(lineText);
            auto node = std::make_shared<ASTNode>(parsed.type, parsed.value, parsed.typeHint, lineNumber++);

            while (!indentStack.empty() && indent <= indentStack.back()) {
                indentStack.pop_back();
                nodeStack.pop_back();
            }

            if (nodeStack.empty()) {
                nodeStack.push_back(node);
            } else {
                nodeStack.back()->children.push_back(node);
                nodeStack.push_back(node);
            }
            indentStack.push_back(indent);
        } catch (const std::exception& e) {
            throw std::runtime_error("Parse error at line " + std::to_string(lineNumber) + ": " + e.what());
        }
    }

    if (nodeStack.empty()) {
        throw std::runtime_error("No valid AST nodes parsed from file: " + filename);
    }

    return nodeStack[0];
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ast_file>.ast" << std::endl;
        return 1;
    }

    try {
        auto ast = readASTFromFile(argv[1]);
        SemanticAnalyzer analyzer(ast);
        analyzer.analyze();
        const auto& issues = analyzer.getIssues();
        bool hasErrors = false;
        for (const auto& issue : issues) {
            if (issue.type == "Error" && issue.description != "No errors found") {
                hasErrors = true;
                break;
            }
        }
        if (!hasErrors) {
            analyzer.saveASTToFile("output2.ast");
            std::cout << "\nOutput Files Generated\n";
            std::cout << "=====================\n";
            std::cout << "- Annotated AST saved to output2.ast\n";
            std::cout << "- Symbol table saved to symbol_table.txt\n";
            std::cout << "- Type checks saved to type_checks.txt\n";
            std::cout << "- Scope checks saved to scope_checks.txt\n\n";
        } else {
            std::cout << "\nSemantic analysis failed. No output files generated.\n\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}