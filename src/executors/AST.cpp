#include "../include/AST.h"
#include <unordered_map>

std::unordered_map<std::string, NodeType> nodeTypeMap = {
    {"Program", NodeType::Program},
    {"Preprocessor", NodeType::Preprocessor},
    {"Struct", NodeType::Struct},
    {"Function", NodeType::Function},
    {"Declarations", NodeType::Declarations},
    {"LocalDeclaration", NodeType::LocalDeclaration},
    {"VarDecl", NodeType::VarDecl},
    {"Assignment", NodeType::Assignment},
    {"While", NodeType::While},
    {"Call", NodeType::Call},
    {"IfElse", NodeType::IfElse},
    {"Return", NodeType::Return},
    {"Identifier", NodeType::Identifier},
    {"Number", NodeType::Number},
    {"String", NodeType::String},
    {"Address", NodeType::Address},
    {"Modulo", NodeType::Modulo},
    {"Equal", NodeType::Equal},
    {"Add", NodeType::Add},
    {"Less", NodeType::Less},
    {"Increment", NodeType::Increment}
};

ASTNode::ASTNode(NodeType t, std::string val, std::string th, std::string cs, int l)
    : type(t), value(val), typeHint(th), callString(cs), line(l) {}