#include "../include/semantic_analyzer.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Map string node types to enum
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
    {"Expression", NodeType::Expression}
};

// Symbol implementation
Symbol::Symbol() : type(""), initialized(false) {
    std::cerr << "Warning: Symbol default constructor used" << std::endl;
}

Symbol::Symbol(const std::string& t) : type(t), initialized(true) {}

// SymbolTable implementation
SymbolTable::SymbolTable() { scopes.push_back({}); } // Global scope

void SymbolTable::enterScope() {
    scopes.push_back({});
    std::cerr << "Debug: Entered scope, total scopes: " << scopes.size() << std::endl;
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
        std::cerr << "Debug: Exited scope, total scopes: " << scopes.size() << std::endl;
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, int line) {
    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        errors.push_back("Error: Redeclaration of variable '" + name + "' at line " + std::to_string(line));
        return false;
    }
    currentScope.emplace(name, Symbol(type));
    std::cerr << "Debug: Declared variable '" << name << "' with type '" << type << "' in scope " << (scopes.size() - 1) << std::endl;
    return true;
}

const Symbol* SymbolTable::lookup(const std::string& name, int line) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto findIt = it->find(name);
        if (findIt != it->end()) {
            return &(findIt->second);
        }
    }
    return nullptr;
}

bool SymbolTable::defineMacro(const std::string& name, const std::string& value, int line) {
    if (macros.find(name) != macros.end()) {
        errors.push_back("Error: Redefinition of macro '" + name + "' at line " + std::to_string(line));
        return false;
    }
    macros[name] = value;
    std::cerr << "Debug: Defined macro '" << name << "' with value '" << value << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineStruct(const std::string& name, int line) {
    if (structs.find(name) != structs.end()) {
        errors.push_back("Error: Redefinition of struct '" + name + "' at line " + std::to_string(line));
        return false;
    }
    structs[name] = true;
    std::cerr << "Debug: Defined struct '" << name << "' at line " << line << std::endl;
    return true;
}

const std::vector<std::string>& SymbolTable::getErrors() const { return errors; }

// ASTNode implementation
ASTNode::ASTNode(NodeType t, std::string val, std::string th, int l)
    : type(t), value(val), typeHint(th), line(l) {}

// SemanticAnalyzer implementation
SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<ASTNode> a) : ast(a) {
    functionSignatures = {
        {"printf", {{"string"}, {"string", "int"}}},
        {"scanf", {{"string", "int*"}}}
    };
}

void SemanticAnalyzer::analyzeNode(const std::shared_ptr<ASTNode>& node) {
    switch (node->type) {
        case NodeType::Program:
            for (const auto& child : node->children) {
                analyzeNode(child);
            }
            break;

        case NodeType::Preprocessor: {
            size_t spacePos = node->value.find(" ");
            if (spacePos == std::string::npos || node->value.substr(0, spacePos) != "#define") {
                errors.push_back("Error: Invalid preprocessor directive at line " + std::to_string(node->line));
                break;
            }
            std::string rest = node->value.substr(spacePos + 1);
            spacePos = rest.find(" ");
            if (spacePos == std::string::npos) {
                errors.push_back("Error: Invalid macro definition at line " + std::to_string(node->line));
                break;
            }
            std::string macroName = rest.substr(0, spacePos);
            std::string macroValue = rest.substr(spacePos + 1);
            symbolTable.defineMacro(macroName, macroValue, node->line);
            break;
        }

        case NodeType::Struct: {
            symbolTable.defineStruct(node->value, node->line);
            break;
        }

        case NodeType::Function:
            symbolTable.enterScope();
            for (const auto& child : node->children) {
                analyzeNode(child);
            }
            symbolTable.exitScope();
            break;

        case NodeType::LocalDeclaration: {
            if (!node->children.empty()) {
                std::string initType = getExpressionType(node->children[0]);
                if (initType != node->typeHint) {
                    errors.push_back("Error: Initializer type mismatch for '" + node->value +
                                     "' at line " + std::to_string(node->line) +
                                     ". Expected " + node->typeHint + ", got " + initType);
                }
            }
            symbolTable.declare(node->value, node->typeHint, node->line);
            break;
        }

        case NodeType::Assignment: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                errors.push_back("Error: Undeclared variable '" + node->value + "' in assignment at line " +
                                 std::to_string(node->line));
                break;
            }
            if (node->children.empty()) {
                errors.push_back("Error: Assignment missing expression at line " + std::to_string(node->line));
                break;
            }
            std::string exprType = getExpressionType(node->children[0]);
            if (exprType != symbol->type) {
                errors.push_back("Error: Assignment type mismatch for '" + node->value +
                                 "' at line " + std::to_string(node->line) +
                                 ". Expected " + symbol->type + ", got " + exprType);
            }
            break;
        }

        case NodeType::While: {
            if (node->children.empty()) {
                errors.push_back("Error: While missing condition at line " + std::to_string(node->line));
                break;
            }
            std::string condType = getExpressionType(node->children[0]);
            if (condType != "bool") {
                errors.push_back("Error: While condition must be boolean at line " + std::to_string(node->line) +
                                 ". Got " + condType);
            }
            for (size_t i = 1; i < node->children.size(); ++i) {
                analyzeNode(node->children[i]);
            }
            break;
        }

        case NodeType::Expression:
            if (node->children.empty()) {
                errors.push_back("Error: Empty expression at line " + std::to_string(node->line));
                break;
            }
            analyzeNode(node->children[0]);
            break;

        case NodeType::Call: {
            std::string funcName = node->value;
            auto it = functionSignatures.find(funcName);
            if (it == functionSignatures.end()) {
                errors.push_back("Error: Unknown function '" + funcName + "' at line " + std::to_string(node->line));
                break;
            }
            bool valid = false;
            for (const auto& expectedTypes : it->second) {
                if (node->children.size() == expectedTypes.size()) {
                    bool argsMatch = true;
                    for (size_t i = 0; i < node->children.size(); ++i) {
                        std::string actualType = getExpressionType(node->children[i]);
                        if (actualType == "unknown") {
                            errors.push_back("Error: Unknown type for argument " + std::to_string(i + 1) +
                                             " in call to '" + funcName + "' at line " + std::to_string(node->line));
                            argsMatch = false;
                            break;
                        }
                        if (actualType != expectedTypes[i]) {
                            errors.push_back("Error: Type mismatch for argument " + std::to_string(i + 1) +
                                             " in call to '" + funcName + "'. Expected " + expectedTypes[i] +
                                             ", got " + actualType + " at line " + std::to_string(node->line));
                            argsMatch = false;
                            break;
                        }
                    }
                    if (argsMatch) {
                        valid = true;
                        break;
                    }
                }
            }
            if (!valid) {
                errors.push_back("Error: Invalid arguments for '" + funcName + "' at line " + std::to_string(node->line));
            }
            break;
        }

        case NodeType::IfElse: {
            if (node->children.size() < 3) {
                errors.push_back("Error: IfElse node missing branches at line " + std::to_string(node->line));
                break;
            }
            std::string condType = getExpressionType(node->children[0]);
            if (condType != "bool") {
                errors.push_back("Error: If condition must be boolean at line " + std::to_string(node->line) +
                                 ". Got " + condType);
            }
            analyzeNode(node->children[1]); // Then branch
            analyzeNode(node->children[2]); // Else branch
            break;
        }

        case NodeType::Return: {
            if (node->children.empty()) {
                errors.push_back("Error: Return node missing expression at line " + std::to_string(node->line));
                break;
            }
            std::string returnType = getExpressionType(node->children[0]);
            if (returnType != "int") {
                errors.push_back("Error: Return type mismatch at line " + std::to_string(node->line) +
                                 ". Expected int, got " + returnType);
            }
            break;
        }

        case NodeType::Increment: {
            if (node->children.empty()) {
                errors.push_back("Error: Increment node missing identifier child at line " + std::to_string(node->line));
                break;
            }
            auto identifierNode = node->children[0];
            if (identifierNode->type != NodeType::Identifier) {
                errors.push_back("Error: Increment node child must be Identifier at line " + std::to_string(node->line));
                break;
            }
            auto symbol = symbolTable.lookup(identifierNode->value, node->line);
            if (!symbol) {
                errors.push_back("Error: Undeclared variable '" + identifierNode->value + "' in increment at line " +
                                 std::to_string(node->line));
                break;
            }
            if (symbol->type != "int") {
                errors.push_back("Error: Increment requires int operand at line " + std::to_string(node->line) +
                                 ". Got " + symbol->type);
                break;
            }
            break;
        }

        default:
            for (const auto& child : node->children) {
                analyzeNode(child);
            }
            break;
    }
}

std::string SemanticAnalyzer::getExpressionType(const std::shared_ptr<ASTNode>& node) const {
    if (!node) {
        errors.push_back("Error: Null node in expression type evaluation");
        return "unknown";
    }
    switch (node->type) {
        case NodeType::Identifier: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                std::cerr << "Debug: Lookup failed for identifier '" << node->value << "' at line " << node->line << std::endl;
                errors.push_back("Error: Undeclared variable '" + node->value + "' at line " + std::to_string(node->line));
                return "unknown";
            }
            std::cerr << "Debug: Lookup succeeded for identifier '" << node->value << "' at line " << node->line << ", type=" << symbol->type << std::endl;
            return symbol->type;
        }
        case NodeType::Number:
            if (node->value.find('.') != std::string::npos) {
                return "float";
            }
            return "int";
        case NodeType::String:
            return "string";
        case NodeType::Address: {
            if (node->children.empty()) {
                errors.push_back("Error: Address node missing operand at line " + std::to_string(node->line));
                return "unknown*";
            }
            std::string baseType = getExpressionType(node->children[0]);
            if (baseType == "unknown") {
                errors.push_back("Error: Unknown type for address operand at line " + std::to_string(node->line));
                return "unknown*";
            }
            return baseType + "*";
        }
        case NodeType::Modulo: {
            if (node->children.size() < 2) {
                errors.push_back("Error: Modulo node missing operands at line " + std::to_string(node->line));
                return "unknown";
            }
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            errors.push_back("Error: Modulo requires int operands at line " + std::to_string(node->line));
            return "unknown";
        }
        case NodeType::Equal: {
            if (node->children.size() < 2) {
                errors.push_back("Error: Equal node missing operands at line " + std::to_string(node->line));
                return "unknown";
            }
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == rightType && leftType != "unknown") {
                return "bool";
            }
            errors.push_back("Error: Type mismatch in comparison at line " + std::to_string(node->line));
            return "unknown";
        }
        case NodeType::Add: {
            if (node->children.size() < 2) {
                errors.push_back("Error: Add node missing operands at line " + std::to_string(node->line));
                return "unknown";
            }
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                return "int";
            }
            errors.push_back("Error: Addition requires int operands at line " + std::to_string(node->line));
            return "unknown";
        }
        case NodeType::Less: {
            if (node->children.size() < 2) {
                errors.push_back("Error: Less node missing operands at line " + std::to_string(node->line));
                return "unknown";
            }
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                return "bool";
            }
            errors.push_back("Error: Less-than comparison requires int operands at line " + std::to_string(node->line));
            return "unknown";
        }
        case NodeType::Increment:
            if (node->children.empty()) {
                errors.push_back("Error: Increment node missing operand at line " + std::to_string(node->line));
                return "unknown";
            }
            return getExpressionType(node->children[0]);
        default:
            errors.push_back("Error: Unsupported expression type '" + std::to_string(static_cast<int>(node->type)) + "' at line " + std::to_string(node->line));
            return "unknown";
    }
}

void SemanticAnalyzer::printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent) const {
    std::string indentStr(indent, ' ');
    std::string nodeStr;

    switch (node->type) {
        case NodeType::Program:
            nodeStr = "Program";
            break;
        case NodeType::Preprocessor:
            nodeStr = "Preprocessor: " + node->value;
            break;
        case NodeType::Struct:
            nodeStr = "Struct: " + node->value;
            break;
        case NodeType::Function:
            nodeStr = "Function: " + node->value + " (" + node->typeHint + ")";
            break;
        case NodeType::LocalDeclaration:
            nodeStr = "LocalDeclaration: " + node->typeHint + " " + node->value + " (type=" + node->typeHint + ")";
            break;
        case NodeType::Assignment:
            nodeStr = "Assignment: " + node->value + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::While:
            nodeStr = "While: " + node->value + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Call: {
            std::string args;
            for (size_t i = 0; i < node->children.size(); ++i) {
                args += getExpressionType(node->children[i]);
                if (i < node->children.size() - 1) args += ",";
            }
            nodeStr = "Call: " + node->value + " (args=" + args + ")";
            break;
        }
        case NodeType::IfElse:
            nodeStr = "IfElse: " + node->value + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Return:
            nodeStr = "Return: " + node->value + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Identifier:
            nodeStr = "Identifier: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Number:
            nodeStr = "Number: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::String:
            nodeStr = "String: \"" + node->value + "\" (type=" + node->typeHint + ")";
            break;
        case NodeType::Address:
            nodeStr = "Address: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Modulo:
            nodeStr = "Modulo: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Equal:
            nodeStr = "Equal: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Add:
            nodeStr = "Add: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Less:
            nodeStr = "Less: " + node->value + " (type=" + getExpressionType(node) + ")";
            break;
        case NodeType::Increment:
            nodeStr = "Increment: " + (node->children.empty() ? node->value : node->children[0]->value) + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Expression:
            nodeStr = "Expression: " + node->value + " (type=" + (node->children.empty() ? "unknown" : getExpressionType(node->children[0])) + ")";
            break;
    }

    out << indentStr << nodeStr << std::endl;
    for (const auto& child : node->children) {
        printAST(child, out, indent + 2);
    }
}

std::vector<std::string> SemanticAnalyzer::analyze() {
    analyzeNode(ast);
    auto symbolErrors = symbolTable.getErrors();
    errors.insert(errors.end(), symbolErrors.begin(), symbolErrors.end());
    return errors;
}

void SemanticAnalyzer::saveASTToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }
    printAST(ast, out);
    out.close();
}

SemanticAnalyzer::~SemanticAnalyzer() {
    // SymbolTable's destructor handles scope cleanup
}

// Helper functions
ParsedNode parseLine(const std::string& line) {
    ParsedNode result;
    std::string cleaned = line;

    // Replace non-standard whitespace (e.g., U+00A0) with standard space
    for (char& c : cleaned) {
        if (static_cast<unsigned char>(c) == 0xA0) {
            c = ' ';
        }
    }

    // Normalize whitespace: replace multiple spaces/tabs with a single space
    std::string normalized;
    bool lastWasSpace = false;
    for (char c : cleaned) {
        if (std::isspace(c)) {
            if (!lastWasSpace) {
                normalized += ' ';
                lastWasSpace = true;
            }
        } else {
            normalized += c;
            lastWasSpace = false;
        }
    }

    // Trim leading and trailing spaces
    normalized.erase(normalized.begin(), std::find_if(normalized.begin(), normalized.end(), [](unsigned char c) { return !std::isspace(c); }));
    normalized.erase(std::find_if(normalized.rbegin(), normalized.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), normalized.end());

    std::cerr << "Debug: Parsing line: '" << normalized << "'" << std::endl;

    // Find the colon separator
    size_t colonPos = normalized.find(": ");
    if (colonPos == std::string::npos) {
        auto it = nodeTypeMap.find(normalized);
        if (it == nodeTypeMap.end()) {
            std::string charCodes;
            for (char c : normalized) {
                charCodes += std::to_string(static_cast<unsigned char>(c)) + " ";
            }
            throw std::runtime_error("Invalid line format or unknown node type: '" + normalized + "' (char codes: " + charCodes + ")");
        }
        result.type = it->second;
        result.value = "";
        result.typeHint = "";
        return result;
    }

    // Extract node type and value
    std::string nodeTypeStr = normalized.substr(0, colonPos);
    std::string rest = normalized.substr(colonPos + 2);

    // Trim node type
    nodeTypeStr.erase(nodeTypeStr.begin(), std::find_if(nodeTypeStr.begin(), nodeTypeStr.end(), [](unsigned char c) { return !std::isspace(c); }));
    nodeTypeStr.erase(std::find_if(nodeTypeStr.rbegin(), nodeTypeStr.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), nodeTypeStr.end());

    // Trim rest
    rest.erase(rest.begin(), std::find_if(rest.begin(), rest.end(), [](unsigned char c) { return !std::isspace(c); }));
    rest.erase(std::find_if(rest.rbegin(), rest.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), rest.end());

    std::cerr << "Debug: Node type: '" << nodeTypeStr << "', Rest: '" << rest << "'" << std::endl;

    auto it = nodeTypeMap.find(nodeTypeStr);
    if (it == nodeTypeMap.end()) {
        std::string charCodes;
        for (char c : nodeTypeStr) {
            charCodes += std::to_string(static_cast<unsigned char>(c)) + " ";
        }
        throw std::runtime_error("Unknown node type: '" + nodeTypeStr + "' in line: '" + normalized + "' (char codes: " + charCodes + ")");
    }
    result.type = it->second;

    if (nodeTypeStr == "Function") {
        size_t parenPos = rest.find(" (");
        if (parenPos != std::string::npos) {
            result.value = rest.substr(0, parenPos);
            result.typeHint = rest.substr(parenPos + 2, rest.size() - parenPos - 3);
        } else {
            throw std::runtime_error("Invalid function format: '" + rest + "'");
        }
    } else if (nodeTypeStr == "LocalDeclaration" || nodeTypeStr == "Declaration") {
        size_t spacePos = rest.find(" = ");
        if (spacePos != std::string::npos) {
            std::string typeAndName = rest.substr(0, spacePos);
            std::string initializer = rest.substr(spacePos + 3);
            size_t typePos = typeAndName.find(" ");
            if (typePos == std::string::npos) {
                throw std::runtime_error("Invalid declaration format: '" + rest + "'");
            }
            result.typeHint = typeAndName.substr(0, typePos);
            result.value = typeAndName.substr(typePos + 1);
            if (initializer.find('.') != std::string::npos) {
                result.typeHint = "float";
                auto child = std::make_shared<ASTNode>(NodeType::Number, initializer, "float", 0);
                result.children.push_back(child);
            } else {
                auto child = std::make_shared<ASTNode>(NodeType::Number, initializer, "int", 0);
                result.children.push_back(child);
            }
        } else {
            size_t spacePos = rest.find(" ");
            if (spacePos != std::string::npos) {
                result.typeHint = rest.substr(0, spacePos);
                result.value = rest.substr(spacePos + 1);
            } else {
                throw std::runtime_error("Invalid declaration format: '" + rest + "'");
            }
        }
    } else if (nodeTypeStr == "Call") {
        size_t parenPos = rest.find("(");
        if (parenPos != std::string::npos) {
            result.value = rest.substr(0, parenPos);
            std::string arg = rest.substr(parenPos + 1, rest.size() - parenPos - 2);
            if (!arg.empty()) {
                auto child = std::make_shared<ASTNode>(NodeType::String, arg, "string", 0);
                result.children.push_back(child);
            }
        } else {
            result.value = rest;
        }
        result.typeHint = "";
    } else if (nodeTypeStr == "Return") {
        result.value = rest;
        auto child = std::make_shared<ASTNode>(NodeType::Number, rest, rest.find('.') != std::string::npos ? "float" : "int", 0);
        result.children.push_back(child);
        result.typeHint = "";
    } else if (nodeTypeStr == "String") {
        result.value = rest;
        if (result.value.find(".n") != std::string::npos) {
            result.value.replace(result.value.find(".n"), 2, "\\n");
        }
        result.typeHint = "string";
    } else if (nodeTypeStr == "Number") {
        result.value = rest;
        result.typeHint = rest.find('.') != std::string::npos ? "float" : "int";
    } else if (nodeTypeStr == "Identifier") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Address") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Preprocessor") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Struct") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Assignment") {
        size_t eqPos = rest.find(" = ");
        if (eqPos != std::string::npos) {
            result.value = rest.substr(0, eqPos);
            std::string expr = rest.substr(eqPos + 3);
            if (expr.find('.') != std::string::npos) {
                auto child = std::make_shared<ASTNode>(NodeType::Number, expr, "float", 0);
                result.children.push_back(child);
            } else {
                auto child = std::make_shared<ASTNode>(NodeType::Number, expr, "int", 0);
                result.children.push_back(child);
            }
            result.typeHint = "";
        } else {
            throw std::runtime_error("Invalid assignment format: '" + rest + "'");
        }
    } else if (nodeTypeStr == "Increment") {
        if (rest.size() >= 2 && rest.substr(rest.size() - 2) == "++") {
            result.value = rest.substr(0, rest.size() - 2);
            auto child = std::make_shared<ASTNode>(NodeType::Identifier, result.value, "", 0);
            result.children.push_back(child);
            result.typeHint = "";
        } else {
            throw std::runtime_error("Invalid increment format: '" + rest + "'. Expected variable++");
        }
    } else if (nodeTypeStr == "While" || nodeTypeStr == "Add" || nodeTypeStr == "Less" ||
               nodeTypeStr == "Expression") {
        result.value = rest;
        result.typeHint = "";
    } else {
        result.value = rest;
        result.typeHint = "";
    }

    return result;
}

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

    for (const auto& line : lines) {
        try {
            int indent = line.first;
            std::string lineText = line.second;
            ParsedNode parsed = parseLine(lineText);
            auto node = std::make_shared<ASTNode>(parsed.type, parsed.value, parsed.typeHint, lineNumber++);
            node->children = parsed.children; // Copy children from ParsedNode

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

    return nodeStack[0]; // Root node (Program)
}