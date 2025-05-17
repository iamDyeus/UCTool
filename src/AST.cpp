#include "AST.h"

std::unordered_map<std::string, NodeType> nodeTypeMap = {
    {"Program", NodeType::Program},
    {"Function", NodeType::Function},
    {"LocalDeclaration", NodeType::LocalDeclaration},
    {"Declaration", NodeType::LocalDeclaration},
    {"Call", NodeType::Call},
    {"IfElse", NodeType::IfElse},
    {"Return", NodeType::Return},
    {"Identifier", NodeType::Identifier},
    {"Number", NodeType::Number},
    {"String", NodeType::String},
    {"Address", NodeType::Address},
    {"Modulo", NodeType::Modulo},
    {"Equal", NodeType::Equal},
    {"Preprocessor", NodeType::Preprocessor},
    {"Struct", NodeType::Struct},
    {"Assignment", NodeType::Assignment},
    {"Add", NodeType::Add},
    {"While", NodeType::While},
    {"Less", NodeType::Less},
    {"Increment", NodeType::Increment},
    {"Expression", NodeType::Increment}
};

ASTNode::ASTNode(NodeType t, std::string val, std::string th, int l)
    : type(t), value(val), typeHint(th), line(l), cachedType("") {}