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
    For,
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
    Subtract,
    Multiply,
    Divide,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    NotEqual,
    Increment,
    PreIncrement,
    PostIncrement,
    CompoundAssign,  // For +=, -=, *=, /=
    Init,           // For loop initialization
    Condition,      // For loop condition
    Update         // For loop update
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