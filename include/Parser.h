#pragma once
#include "AST.h"
#include <string>
#include <memory>

// Structure to hold parsed node information
struct ParsedNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    ParsedNode(NodeType t, const std::string& val, const std::string& th, int l);
};

// Function to parse a line into a ParsedNode
ParsedNode parseLine(const std::string& line, int lineNumber);

// Function to read AST from file
std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename);