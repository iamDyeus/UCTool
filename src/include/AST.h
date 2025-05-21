#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class NodeType {
    Program,
    Preprocessor,
    Struct,
    Function,
    Declarations,
    LocalDeclaration,
    VarDecl,
    Assignment,
    While,
    Call,
    IfElse,
    Return,
    Identifier,
    Number,
    String,
    Address,
    Modulo,
    Equal,
    Add,
    Less,
    Increment
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    std::string callString; // Store full call string for Call nodes
    int line;
    std::string cachedType;
    std::vector<std::shared_ptr<ASTNode>> children;
    
    ASTNode(NodeType t, std::string val = "", std::string th = "", std::string cs = "", int l = 1);
};

// Declare nodeTypeMap as extern to be defined in AST.cpp
extern std::unordered_map<std::string, NodeType> nodeTypeMap;