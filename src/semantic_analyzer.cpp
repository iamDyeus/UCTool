#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Symbol table entry
struct Symbol {
    std::string type;
    bool initialized;
    Symbol() : type(""), initialized(false) {
        std::cerr << "Warning: Symbol default constructor used" << std::endl;
    }
    Symbol(const std::string& t) : type(t), initialized(true) {}
};

// Symbol table with scope management
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<std::string> errors;

public:
    SymbolTable() { scopes.push_back({}); } // Global scope

    void enterScope() {
        scopes.push_back({});
        std::cerr << "Debug: Entered scope, total scopes: " << scopes.size() << std::endl;
    }

    void exitScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
            std::cerr << "Debug: Exited scope, total scopes: " << scopes.size() << std::endl;
        }
    }

    bool declare(const std::string& name, const std::string& type, int line) {
        auto& currentScope = scopes.back();
        if (currentScope.find(name) != currentScope.end()) {
            errors.push_back("Error: Redeclaration of variable '" + name + "' at line " + std::to_string(line));
            return false;
        }
        currentScope.emplace(name, Symbol(type));
        std::cerr << "Debug: Declared variable '" << name << "' with type '" << type << "' in scope " << scopes.size() - 1 << std::endl;
        return true;
    }

    const Symbol* lookup(const std::string& name, int line) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto findIt = it->find(name);
            if (findIt != it->end()) {
                return &(findIt->second);
            }
        }
        return nullptr; // Errors handled in caller
    }

    const std::vector<std::string>& getErrors() const { return errors; }
};

// AST node types
enum class NodeType {
    Program, Function, LocalDeclaration, Call, IfElse, Return,
    Identifier, Number, String, Address, Modulo, Equal
};

// Map string node types to enum
std::unordered_map<std::string, NodeType> nodeTypeMap = {
    {"Program", NodeType::Program},
    {"Function", NodeType::Function},
    {"LocalDeclaration", NodeType::LocalDeclaration},
    {"Call", NodeType::Call},
    {"IfElse", NodeType::IfElse},
    {"Return", NodeType::Return},
    {"Identifier", NodeType::Identifier},
    {"Number", NodeType::Number},
    {"String", NodeType::String},
    {"Address", NodeType::Address},
    {"Modulo", NodeType::Modulo},
    {"Equal", NodeType::Equal}
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::string typeHint;
    int line;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(NodeType t, std::string val = "", std::string th = "", int l = 1)
        : type(t), value(val), typeHint(th), line(l) {}
};

// Semantic analyzer
class SemanticAnalyzer {
private:
    std::shared_ptr<ASTNode> ast;
    SymbolTable symbolTable;
    mutable std::vector<std::string> errors; // Mutable to allow error collection in const methods
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> functionSignatures = {
        {"printf", {{"string"}, {"string", "int"}}},
        {"scanf", {{"string", "int*"}}}
    };

    void analyzeNode(const std::shared_ptr<ASTNode>& node) {
        switch (node->type) {
            case NodeType::Program:
                for (const auto& child : node->children) {
                    analyzeNode(child);
                }
                break;

            case NodeType::Function:
                symbolTable.enterScope();
                for (const auto& child : node->children) {
                    analyzeNode(child);
                }
                // Scope preserved for printAST
                break;

            case NodeType::LocalDeclaration:
                symbolTable.declare(node->value, node->typeHint, node->line);
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
                auto condition = node->children[0];
                std::string condType = getExpressionType(condition);
                if (condType != "bool") {
                    errors.push_back("Error: If condition must be boolean at line " + std::to_string(node->line) +
                                     ". Got " + condType);
                }
                analyzeNode(node->children[1]); // Then branch
                analyzeNode(node->children[2]); // Else branch
                break;
            }

            case NodeType::Return: {
                std::string returnType = getExpressionType(node->children[0]);
                if (returnType != "int") {
                    errors.push_back("Error: Return type mismatch at line " + std::to_string(node->line) +
                                     ". Expected int, got " + returnType);
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

    std::string getExpressionType(const std::shared_ptr<ASTNode>& node) const {
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
                return "int";
            case NodeType::String:
                return "string";
            case NodeType::Address: {
                std::string baseType = getExpressionType(node->children[0]);
                if (baseType == "unknown") {
                    errors.push_back("Error: Unknown type for address operand at line " + std::to_string(node->line));
                    return "unknown*";
                }
                return baseType + "*";
            }
            case NodeType::Modulo: {
                auto leftType = getExpressionType(node->children[0]);
                auto rightType = getExpressionType(node->children[1]);
                if (leftType == "int" && rightType == "int") {
                    return "int";
                }
                errors.push_back("Error: Modulo requires int operands at line " + std::to_string(node->line));
                return "unknown";
            }
            case NodeType::Equal: {
                auto leftType = getExpressionType(node->children[0]);
                auto rightType = getExpressionType(node->children[1]);
                if (leftType == rightType && leftType != "unknown") {
                    return "bool";
                }
                errors.push_back("Error: Type mismatch in comparison at line " + std::to_string(node->line));
                return "unknown";
            }
            default:
                errors.push_back("Error: Unsupported expression type '" + std::to_string(static_cast<int>(node->type)) + "' at line " + std::to_string(node->line));
                return "unknown";
        }
    }

    void printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent = 0) const {
        std::string indentStr(indent, ' ');
        std::string nodeStr;

        switch (node->type) {
            case NodeType::Program:
                nodeStr = "Program";
                break;
            case NodeType::Function:
                nodeStr = "Function: " + node->value + " (" + node->typeHint + ")";
                break;
            case NodeType::LocalDeclaration:
                nodeStr = "LocalDeclaration: " + node->typeHint + " " + node->value + " (type=" + node->typeHint + ")";
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
                nodeStr = "Number: " + node->value + " (type=" + node->typeHint + ")";
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
        }

        out << indentStr << nodeStr << std::endl;
        for (const auto& child : node->children) {
            printAST(child, out, indent + 2);
        }
    }

public:
    SemanticAnalyzer(std::shared_ptr<ASTNode> a) : ast(a) {}

    std::vector<std::string> analyze() {
        analyzeNode(ast);
        auto symbolErrors = symbolTable.getErrors();
        errors.insert(errors.end(), symbolErrors.begin(), symbolErrors.end());
        return errors;
    }

    void saveASTToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out.is_open()) {
            throw std::runtime_error("Failed to open output file: " + filename);
        }
        printAST(ast, out);
        out.close();
    }

    ~SemanticAnalyzer() {
        // Clean up all scopes
        while (!symbolTable.getErrors().empty() || symbolTable.getErrors().size() < scopes.size()) {
            symbolTable.exitScope();
        }
    }
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

// Parse a single line into node type, value, and typeHint
struct ParsedNode {
    NodeType type;
    std::string value;
    std::string typeHint;
};

ParsedNode parseLine(const std::string& line) {
    ParsedNode result;
    std::string trimmed = line;
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char c) { return !std::isspace(c); }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), trimmed.end());

    std::cerr << "Debug: Parsing line: '" << trimmed << "'" << std::endl;

    size_t colonPos = trimmed.find(": ");
    if (colonPos == std::string::npos) {
        auto it = nodeTypeMap.find(trimmed);
        if (it == nodeTypeMap.end()) {
            throw std::runtime_error("Unknown node type: '" + trimmed + "'");
        }
        result.type = it->second;
        result.value = "";
        result.typeHint = "";
        return result;
    }

    std::string nodeTypeStr = trimmed.substr(0, colonPos);
    std::string rest = trimmed.substr(colonPos + 2);

    auto it = nodeTypeMap.find(nodeTypeStr);
    if (it == nodeTypeMap.end()) {
        throw std::runtime_error("Unknown node type: '" + nodeTypeStr + "'");
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
    } else if (nodeTypeStr == "LocalDeclaration") {
        size_t spacePos = rest.find(" ");
        if (spacePos != std::string::npos) {
            result.typeHint = rest.substr(0, spacePos);
            result.value = rest.substr(spacePos + 1);
        } else {
            throw std::runtime_error("Invalid declaration format: '" + rest + "'");
        }
    } else if (nodeTypeStr == "Call") {
        size_t parenPos = rest.find("(");
        if (parenPos != std::string::npos) {
            result.value = rest.substr(0, parenPos);
        } else {
            result.value = rest;
        }
        result.typeHint = "";
    } else if (nodeTypeStr == "String") {
        result.value = rest;
        if (result.value.find(".n") != std::string::npos) {
            result.value.replace(result.value.find(".n"), 2, "\\n");
        }
        result.typeHint = "string";
    } else if (nodeTypeStr == "Number") {
        result.value = rest;
        result.typeHint = "int";
    } else if (nodeTypeStr == "Identifier") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Address") {
        result.value = rest;
        result.typeHint = "";
    } else {
        result.value = rest;
        result.typeHint = "";
    }

    return result;
}

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
            if (c == ' ' || c == '\t') indentLevel++;
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

    return nodeStack[0]; // Root node (Program)
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ast_file>.ast" << std::endl;
        return 1;
    }

    try {
        auto ast = readASTFromFile(argv[1]);
        SemanticAnalyzer analyzer(ast);
        auto errors = analyzer.analyze();
        for (const auto& error : errors) {
            std::cout << error << std::endl;
        }
        if (errors.empty()) {
            std::cout << "Semantic analysis passed." << std::endl;
            analyzer.saveASTToFile("output.ast");
            std::cout << "Annotated AST saved to output.ast" << std::endl;
        } else {
            std::cout << "Semantic analysis failed. No output file generated." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}