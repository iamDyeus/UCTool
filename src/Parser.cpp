#include "Parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

ParsedNode::ParsedNode(NodeType t, const std::string& val, const std::string& th, int l)
    : type(t), value(val), typeHint(th), line(l) {}

ParsedNode parseLine(const std::string& line, int lineNumber) {
    std::string trimmed = line;
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char c) { return !std::isspace(c); }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), trimmed.end());

    if (trimmed.empty()) {
        throw std::runtime_error("Empty line at line " + std::to_string(lineNumber));
    }

    // Skip artifact lines (e.g., EOF, terminal prompts)
    if (trimmed == "EOF" || trimmed.find("@") != std::string::npos) {
        throw std::runtime_error("Invalid line artifact: '" + trimmed + "' at line " + std::to_string(lineNumber));
    }

    std::string nodeTypeStr;
    std::string rest;
    size_t colonPos = trimmed.find(':');
    if (colonPos == std::string::npos) {
        // No colon: treat entire line as node type with empty value
        nodeTypeStr = trimmed;
        rest = "";
        std::cout << "Debug: Parsed line " << lineNumber << ": No colon, Type=" << nodeTypeStr << ", Value=, TypeHint=" << std::endl;
    } else {
        // Colon present: split into node type and rest
        nodeTypeStr = trimmed.substr(0, colonPos);
        rest = trimmed.substr(colonPos + 1);
        rest.erase(rest.begin(), std::find_if(rest.begin(), rest.end(), [](unsigned char c) { return !std::isspace(c); }));
        std::cout << "Debug: Parsed line " << lineNumber << ": Colon found, Type=" << nodeTypeStr << ", Rest=" << rest << std::endl;
    }

    if (nodeTypeStr.empty()) {
        throw std::runtime_error("Empty node type in line: '" + trimmed + "' at line " + std::to_string(lineNumber));
    }

    auto it = nodeTypeMap.find(nodeTypeStr);
    if (it == nodeTypeMap.end()) {
        throw std::runtime_error("Unknown node type: '" + nodeTypeStr + "' at line " + std::to_string(lineNumber));
    }

    NodeType nodeType = it->second;
    std::string value = rest;
    std::string typeHint;

    if (nodeType == NodeType::Function) {
        size_t parenPos = rest.find('(');
        if (parenPos != std::string::npos) {
            value = rest.substr(0, parenPos);
            size_t endParenPos = rest.find(')', parenPos);
            if (endParenPos != std::string::npos) {
                typeHint = rest.substr(parenPos + 1, endParenPos - parenPos - 1);
            }
        }
    } else if (nodeType == NodeType::LocalDeclaration) {
        size_t spacePos = rest.find(' ');
        if (spacePos != std::string::npos) {
            typeHint = rest.substr(0, spacePos);
            value = rest.substr(spacePos + 1);
            size_t equalsPos = value.find('=');
            if (equalsPos != std::string::npos) {
                value = value.substr(0, equalsPos);
                value.erase(value.find_last_not_of(" \t") + 1);
            }
        }
    } else if (nodeType == NodeType::Call) {
        size_t parenPos = rest.find('(');
        if (parenPos != std::string::npos) {
            value = rest.substr(0, parenPos);
        }
    } else if (nodeTypeStr == "Expression") {
        if (rest.size() >= 2 && rest.substr(rest.size() - 2) == "++") {
            nodeType = NodeType::Increment;
            value = rest.substr(0, rest.size() - 2);
            typeHint = "";
        } else {
            throw std::runtime_error("Invalid expression format: '" + rest + "' at line " + std::to_string(lineNumber));
        }
    }

    std::cout << "Debug: Final parsed line " << lineNumber << ": Type=" << nodeTypeStr << ", Value=" << value << ", TypeHint=" << typeHint << std::endl;

    return ParsedNode(nodeType, value, typeHint, lineNumber);
}

std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<std::shared_ptr<ASTNode>> nodeStack;
    std::vector<int> indentStack;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;
        try {
            if (line.empty()) {
                continue;
            }

            int indent = 0;
            while (indent < line.length() && line[indent] == ' ') {
                indent++;
            }

            auto parsed = parseLine(line, lineNumber);
            auto node = std::make_shared<ASTNode>(parsed.type, parsed.value, parsed.typeHint, parsed.line);

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
            file.close();
            throw std::runtime_error("Parse error at line " + std::to_string(lineNumber) + ": " + e.what());
        }
    }

    file.close();

    if (nodeStack.empty()) {
        throw std::runtime_error("No valid AST nodes parsed from file: " + filename);
    }

    return nodeStack[0];
}