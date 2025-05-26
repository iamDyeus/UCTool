#pragma once
#include "AST.h"
#include <string>
#include <memory>

struct ParsedNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    std::string callString; // Store full call string for Call nodes
    int line;
    ParsedNode(NodeType t, const std::string& val, const std::string& th, const std::string& cs, int l);
};

std::shared_ptr<ASTNode> readASTFromFile(const std::string& filename);