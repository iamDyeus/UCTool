#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// AST node types
enum class NodeType {
    Program, Function, LocalDeclaration, Call, IfElse, Return,
    Identifier, Number, String, Address, Modulo, Equal,
    Preprocessor, Struct, Assignment, Add, While, Less, Increment
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    std::vector<std::shared_ptr<ASTNode>> children;
    mutable std::string cachedType;
    ASTNode(NodeType t, std::string val = "", std::string th = "", int l = 1);
};

extern std::unordered_map<std::string, NodeType> nodeTypeMap;