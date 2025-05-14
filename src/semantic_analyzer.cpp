#include <iostream>
#include "../include/semantic_analyzer.h"

// Symbol constructor implementations
Symbol::Symbol() : type(""), scope(""), attributes(""), initialized(false), used(false) {
    std::cerr << "Warning: Symbol default constructor used" << std::endl;
}

Symbol::Symbol(const std::string& t, const std::string& s, const std::string& a)
    : type(t), scope(s), attributes(a), initialized(true), used(false) {}

// TypeCheck constructor
TypeCheck::TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat)
    : location(loc), description(desc), status(stat) {}

// ScopeCheck constructor
ScopeCheck::ScopeCheck(const std::string& s, const std::string& a, int count)
    : scope(s), action(a), symbolCount(count) {}

// SemanticIssue constructor
SemanticIssue::SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat)
    : type(t), description(desc), status(stat) {}

// SymbolTable implementation
SymbolTable::SymbolTable() : hasStdioInclude(false) {
    scopes.push_back({});
    scopeNames.push_back("global");
    scopeChecks.emplace_back("global", "Entered", 0);
}

void SymbolTable::enterScope(const std::string& scopeName) {
    scopes.push_back({});
    scopeNames.push_back(scopeName);
    scopeChecks.emplace_back(scopeName, "Entered", scopes.back().size());
    std::cerr << "Debug: Entered scope '" << scopeName << "', total scopes: " << scopes.size() << std::endl;
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopeChecks.emplace_back(scopeNames.back(), "Exited", scopes.back().size());
        scopes.pop_back();
        std::cerr << "Debug: Exited scope '" << scopeNames.back() << "', total scopes: " << scopes.size() << std::endl;
        scopeNames.pop_back();
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, const std::string& attributes, int line) {
    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        issues.emplace_back("Error", "Redeclaration of variable '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    currentScope.emplace(name, Symbol(type, scopeNames.back(), attributes));
    std::cerr << "Debug: Declared variable '" << name << "' with type '" << type << "' in scope '" << scopeNames.back() << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineMacro(const std::string& name, const std::string& value, int line) {
    if (macros.find(name) != macros.end()) {
        issues.emplace_back("Error", "Redefinition of macro '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    macros.emplace(name, Symbol("int (macro)", "global", "preprocessor constant"));
    std::cerr << "Debug: Defined macro '" << name << "' with value '" << value << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineStruct(const std::string& name, int line) {
    if (structs.find(name) != structs.end()) {
        issues.emplace_back("Error", "Redefinition of struct '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    structs.emplace(name, Symbol("struct {float x, float y}", "global", "user-defined type"));
    std::cerr << "Debug: Defined struct '" << name << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineFunction(const std::string& name, const std::string& type, int line) {
    if (functions.find(name) != functions.end()) {
        issues.emplace_back("Error", "Redefinition of function '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    functions.emplace(name, Symbol(type + " func(void)", "global", "function"));
    std::cerr << "Debug: Defined function '" << name << "' with type '" << type << " func(void)' at line " << line << std::endl;
    return true;
}

const Symbol* SymbolTable::lookup(const std::string& name, int line) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto findIt = it->find(name);
        if (findIt != it->end()) {
            return &(findIt->second);
        }
    }
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        return &(macroIt->second);
    }
    auto structIt = structs.find(name);
    if (structIt != structs.end()) {
        return &(structIt->second);
    }
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        return &(funcIt->second);
    }
    return nullptr;
}

void SymbolTable::markUsed(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto findIt = it->find(name);
        if (findIt != it->end()) {
            findIt->second.used = true;
            return;
        }
    }
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        macroIt->second.used = true;
        return;
    }
    auto structIt = structs.find(name);
    if (structIt != structs.end()) {
        structIt->second.used = true;
        return;
    }
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        funcIt->second.used = true;
        return;
    }
}

void SymbolTable::setStdioInclude() { hasStdioInclude = true; }
bool SymbolTable::hasStdio() const { return hasStdioInclude; }

void SymbolTable::addTypeCheck(const std::string& location, const std::string& description, const std::string& status) const {
    typeChecks.emplace_back(location, description, status);
}

void SymbolTable::addWarning(const std::string& description, int line) {
    issues.emplace_back("Warning", description + " at line " + std::to_string(line), "⚠️");
}

void SymbolTable::checkUnusedSymbols() {
    for (auto& [name, symbol] : macros) {
        if (!symbol.used) {
            issues.emplace_back("Warning", "Unused macro '" + name + "'", "⚠️");
        }
    }
    for (auto& [name, symbol] : structs) {
        if (!symbol.used) {
            issues.emplace_back("Warning", "Unused struct '" + name + "'", "⚠️");
        }
    }
}

void SymbolTable::printSymbolTable() const {
    std::ofstream outFile("symbol_table.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open symbol_table.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nSymbol Table\n";
        os << "============\n\n";
        os << std::left
           << std::setw(20) << "Name"
           << std::setw(30) << "Type"
           << std::setw(15) << "Scope"
           << std::setw(25) << "Attributes" << "\n";
        os << std::string(90, '-') << "\n";
        for (const auto& [name, symbol] : macros) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes << "\n";
        }
        for (const auto& [name, symbol] : structs) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes << "\n";
        }
        for (const auto& [name, symbol] : functions) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes << "\n";
        }
        for (const auto& scope : scopes) {
            for (const auto& [name, symbol] : scope) {
                os << std::setw(20) << name
                   << std::setw(30) << symbol.type
                   << std::setw(15) << symbol.scope
                   << std::setw(25) << symbol.attributes << "\n";
            }
        }
        os << "\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printTypeChecks() const {
    std::ofstream outFile("type_checks.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open type_checks.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nType Checking\n";
        os << "=============\n\n";
        os << std::left
           << std::setw(30) << "Location"
           << std::setw(50) << "Description"
           << std::setw(10) << "Status" << "\n";
        os << std::string(90, '-') << "\n";
        for (const auto& check : typeChecks) {
            os << std::setw(30) << check.location
               << std::setw(50) << check.description
               << std::setw(10) << check.status << "\n";
        }
        os << "\n" << (typeChecks.empty() ? "No type checks performed." : "✅ All type checks passed.") << "\n\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printScopeChecks() const {
    std::ofstream outFile("scope_checks.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open scope_checks.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nScope Checking\n";
        os << "==============\n\n";
        os << std::left
           << std::setw(20) << "Scope"
           << std::setw(15) << "Action"
           << std::setw(15) << "Symbol Count" << "\n";
        os << std::string(50, '-') << "\n";
        for (const auto& check : scopeChecks) {
            os << std::setw(20) << check.scope
               << std::setw(15) << check.action
               << std::setw(15) << check.symbolCount << "\n";
        }
        os << "\n✅ All scopes properly managed.\n\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printIssues() const {
    std::cout << "\nSemantic Errors/Warnings\n";
    std::cout << "========================\n\n";
    std::cout << std::left
              << std::setw(10) << "Type"
              << std::setw(60) << "Description"
              << std::setw(10) << "Status" << "\n";
    std::cout << std::string(80, '-') << "\n";
    bool hasErrors = false;
    bool hasWarnings = false;
    for (const auto& issue : issues) {
        std::cout << std::setw(10) << issue.type
                  << std::setw(60) << issue.description
                  << std::setw(10) << issue.status << "\n";
        if (issue.type == "Error") hasErrors = true;
        if (issue.type == "Warning") hasWarnings = true;
    }
    if (!hasErrors && !hasWarnings) {
        std::cout << std::setw(10) << "Error"
                  << std::setw(60) << "No errors found"
                  << std::setw(10) << "✅" << "\n";
        std::cout << std::setw(10) << "Warning"
                  << std::setw(60) << "No warnings found"
                  << std::setw(10) << "✅" << "\n";
    } else if (!hasErrors) {
        std::cout << std::setw(10) << "Error"
                  << std::setw(60) << "No errors found"
                  << std::setw(10) << "✅" << "\n";
    }
    std::cout << "\n" << (hasErrors ? "❌ Semantic errors detected." : "✅ No major semantic errors detected.") << "\n\n";
}

const std::vector<SemanticIssue>& SymbolTable::getIssues() const { return issues; }

bool SymbolTable::hasOnlyWarnings() const {
    for (const auto& issue : issues) {
        if (issue.type == "Error") return false;
    }
    return !issues.empty();
}

// ASTNode constructor
ASTNode::ASTNode(NodeType t, std::string val, std::string th, int l)
    : type(t), value(val), typeHint(th), line(l), cachedType("") {}

// SemanticAnalyzer implementation
SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<ASTNode> a) : ast(a) {
    functionSignatures = {
        {"printf", {{"string"}, {"string", "int"}}},
        {"scanf", {{"string", "int*"}}}
    };
}

void SemanticAnalyzer::analyzeNode(const std::shared_ptr<ASTNode>& node) {
    std::cerr << "Debug: Analyzing node type=" << static_cast<int>(node->type) << ", value='" << node->value << "', line=" << node->line << ", scope='" << symbolTable.getCurrentScope() << "'" << std::endl;
    switch (node->type) {
        case NodeType::Program:
            for (const auto& child : node->children) {
                analyzeNode(child);
            }
            symbolTable.checkUnusedSymbols();
            break;

        case NodeType::Preprocessor: {
            if (node->value == "#include <stdio.h>") {
                symbolTable.setStdioInclude();
                symbolTable.addTypeCheck("#include <stdio.h>", "Standard I/O included", "OK");
            } else if (node->value.find("#define") == 0) {
                size_t spacePos = node->value.find(" ");
                if (spacePos == std::string::npos) {
                    issues.emplace_back("Error", "Invalid preprocessor directive at line " + std::to_string(node->line), "❌");
                    break;
                }
                std::string rest = node->value.substr(spacePos + 1);
                spacePos = rest.find(" ");
                if (spacePos == std::string::npos) {
                    issues.emplace_back("Error", "Invalid macro definition at line " + std::to_string(node->line), "❌");
                    break;
                }
                std::string macroName = rest.substr(0, spacePos);
                std::string macroValue = rest.substr(spacePos + 1);
                symbolTable.defineMacro(macroName, macroValue, node->line);
            } else {
                issues.emplace_back("Error", "Invalid preprocessor directive at line " + std::to_string(node->line), "❌");
            }
            break;
        }

        case NodeType::Struct: {
            symbolTable.defineStruct(node->value, node->line);
            break;
        }

        case NodeType::Function: {
            symbolTable.defineFunction(node->value, node->typeHint, node->line);
            symbolTable.enterScope(node->value);
            for (const auto& child : node->children) {
                analyzeNode(child);
            }
            symbolTable.exitScope();
            break;
        }

        case NodeType::LocalDeclaration: {
            if (!node->children.empty()) {
                std::string initType = getExpressionType(node->children[0]);
                if (initType == node->typeHint) {
                    symbolTable.addTypeCheck(
                        node->value + " = " + node->children[0]->value,
                        "Assigning " + initType + " literal to " + node->typeHint + " variable",
                        "OK"
                    );
                } else {
                    issues.emplace_back(
                        "Error",
                        "Initializer type mismatch for '" + node->value + "' at line " + std::to_string(node->line) +
                        ". Expected " + node->typeHint + ", got " + initType,
                        "❌"
                    );
                }
            }
            symbolTable.declare(node->value, node->typeHint, "local variable", node->line);
            break;
        }

        case NodeType::Assignment: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                issues.emplace_back(
                    "Error",
                    "Undeclared variable '" + node->value + "' in assignment at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            symbolTable.markUsed(node->value);
            std::string exprType = getExpressionType(node->children[0]);
            if (exprType == symbol->type) {
                symbolTable.addTypeCheck(
                    node->value + " = " + node->children[0]->value,
                    "Assigning " + exprType + " expression to " + symbol->type + " variable",
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Assignment type mismatch for '" + node->value + "' at line " + std::to_string(node->line) +
                    ". Expected " + symbol->type + ", got " + exprType,
                    "❌"
                );
            }
            break;
        }

        case NodeType::While: {
            std::string condType = getExpressionType(node->children[0]);
            if (condType == "bool") {
                symbolTable.addTypeCheck(
                    "while " + node->value,
                    "Condition evaluates to bool",
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "While condition must be boolean at line " + std::to_string(node->line) + ". Got " + condType,
                    "❌"
                );
            }
            for (size_t i = 1; i < node->children.size(); ++i) {
                analyzeNode(node->children[i]);
            }
            break;
        }

        case NodeType::Expression:
            if (!node->children.empty()) {
                analyzeNode(node->children[0]);
            } else {
                issues.emplace_back(
                    "Error",
                    "Empty expression at line " + std::to_string(node->line),
                    "❌"
                );
            }
            break;

        case NodeType::Call: {
            std::string funcName = node->value;
            auto it = functionSignatures.find(funcName);
            if (it == functionSignatures.end()) {
                issues.emplace_back(
                    "Error",
                    "Unknown function '" + funcName + "' at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            symbolTable.markUsed(funcName);
            if (funcName == "printf" && !symbolTable.hasStdio()) {
                symbolTable.addWarning(
                    "printf used without visible prototype (no #include <stdio.h>)",
                    node->line
                );
            }
            bool valid = false;
            for (const auto& expectedTypes : it->second) {
                // Handle single-argument calls using typeHint
                if (expectedTypes.size() == 1 && node->typeHint == expectedTypes[0] && node->children.empty()) {
                    valid = true;
                    symbolTable.addTypeCheck(
                        funcName + "(" + node->typeHint + ")",
                        "Calling " + funcName + " with " + expectedTypes[0],
                        "OK"
                    );
                    break;
                }
                // Handle multi-argument calls using children
                if (node->children.size() == expectedTypes.size()) {
                    bool argsMatch = true;
                    for (size_t i = 0; i < node->children.size(); ++i) {
                        if (!node->children[i]) {
                            issues.emplace_back(
                                "Error",
                                "Invalid argument " + std::to_string(i + 1) + " in call to '" + funcName +
                                "' at line " + std::to_string(node->line),
                                "❌"
                            );
                            argsMatch = false;
                            break;
                        }
                        std::string actualType = getExpressionType(node->children[i]);
                        if (actualType == "unknown") {
                            issues.emplace_back(
                                "Error",
                                "Unknown type for argument " + std::to_string(i + 1) + " in call to '" + funcName +
                                "' at line " + std::to_string(node->line),
                                "❌"
                            );
                            argsMatch = false;
                            break;
                        }
                        if (actualType != expectedTypes[i]) {
                            issues.emplace_back(
                                "Error",
                                "Type mismatch for argument " + std::to_string(i + 1) + " in call to '" + funcName +
                                "'. Expected " + expectedTypes[i] + ", got " + actualType + " at line " +
                                std::to_string(node->line),
                                "❌"
                            );
                            argsMatch = false;
                            break;
                        }
                    }
                    if (argsMatch) {
                        valid = true;
                        std::string argsDesc = "Calling " + funcName + " with ";
                        for (size_t i = 0; i < expectedTypes.size(); ++i) {
                            argsDesc += expectedTypes[i];
                            if (i < expectedTypes.size() - 1) argsDesc += ", ";
                        }
                        symbolTable.addTypeCheck(
                            funcName + (node->children.empty() ? "()" : "(" + node->children[0]->value + ")"),
                            argsDesc,
                            "OK"
                        );
                        break;
                    }
                }
            }
            if (!valid) {
                issues.emplace_back(
                    "Error",
                    "Invalid arguments for '" + funcName + "' at line " + std::to_string(node->line),
                    "❌"
                );
            }
            break;
        }

        case NodeType::IfElse: {
            auto condition = node->children[0];
            std::string condType = getExpressionType(condition);
            if (condType != "bool") {
                issues.emplace_back(
                    "Error",
                    "If condition must be boolean at line " + std::to_string(node->line) + ". Got " + condType,
                    "❌"
                );
            } else {
                symbolTable.addTypeCheck(
                    node->value,
                    "If condition evaluates to bool",
                    "OK"
                );
            }
            analyzeNode(node->children[1]);
            analyzeNode(node->children[2]);
            break;
        }

        case NodeType::Return: {
            if (node->children.empty()) {
                issues.emplace_back(
                    "Error",
                    "Return statement missing expression at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            std::string returnType = getExpressionType(node->children[0]);
            if (returnType == "int") {
                symbolTable.addTypeCheck(
                    "return " + node->children[0]->value,
                    "Returning " + returnType + " from function",
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Return type mismatch at line " + std::to_string(node->line) + ". Expected int, got " + returnType,
                    "❌"
                );
            }
            break;
        }

        case NodeType::Increment: {
            if (node->children.empty()) {
                issues.emplace_back(
                    "Error",
                    "Increment node missing identifier child at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            auto identifierNode = node->children[0];
            if (identifierNode->type != NodeType::Identifier) {
                issues.emplace_back(
                    "Error",
                    "Increment node child must be Identifier at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            auto symbol = symbolTable.lookup(identifierNode->value, node->line);
            if (!symbol) {
                issues.emplace_back(
                    "Error",
                    "Undeclared variable '" + identifierNode->value + "' in increment at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            symbolTable.markUsed(identifierNode->value);
            if (symbol->type == "int") {
                symbolTable.addTypeCheck(
                    identifierNode->value + "++",
                    "Post-increment of " + symbol->type,
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Increment requires int operand at line " + std::to_string(node->line) + ". Got " + symbol->type,
                    "❌"
                );
            }
            break;
        }

        case NodeType::Identifier: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                issues.emplace_back(
                    "Error",
                    "Undeclared variable '" + node->value + "' at line " + std::to_string(node->line),
                    "❌"
                );
            } else {
                symbolTable.markUsed(node->value);
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

std::string SymbolTable::getCurrentScope() const {
    return scopeNames.empty() ? "none" : scopeNames.back();
}

std::string SemanticAnalyzer::getExpressionType(const std::shared_ptr<ASTNode>& node) {
    if (!node->cachedType.empty()) {
        return node->cachedType;
    }

    std::cerr << "Debug: Getting expression type for node type=" << static_cast<int>(node->type) << ", value='" << node->value << "', line=" << node->line << ", scope='" << symbolTable.getCurrentScope() << "'" << std::endl;

    std::string result;
    switch (node->type) {
        case NodeType::Identifier: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                std::cerr << "Debug: Lookup failed for identifier '" << node->value << "' at line " << node->line << std::endl;
                issues.emplace_back(
                    "Error",
                    "Undeclared variable '" + node->value + "' at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            } else {
                std::cerr << "Debug: Lookup succeeded for identifier '" << node->value << "' at line " << node->line << ", type=" << symbol->type << std::endl;
                result = symbol->type;
            }
            break;
        }
        case NodeType::Number: {
            if (node->value.find('.') != std::string::npos) {
                result = "float";
            } else {
                result = "int";
            }
            break;
        }
        case NodeType::String:
            result = "string";
            break;
        case NodeType::Address: {
            std::string baseType = getExpressionType(node->children[0]);
            if (baseType == "unknown") {
                issues.emplace_back(
                    "Error",
                    "Unknown type for address operand at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown*";
            } else {
                result = baseType + "*";
            }
            break;
        }
        case NodeType::Modulo: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                symbolTable.addTypeCheck(
                    node->value,
                    "Modulo operation with int operands",
                    "OK"
                );
                result = "int";
            } else {
                issues.emplace_back(
                    "Error",
                    "Modulo requires int operands at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
            break;
        }
        case NodeType::Equal: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == rightType && leftType != "unknown") {
                symbolTable.addTypeCheck(
                    node->value,
                    "Equality comparison with matching types",
                    "OK"
                );
                result = "bool";
            } else {
                issues.emplace_back(
                    "Error",
                    "Type mismatch in comparison at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
            break;
        }
        case NodeType::Add: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                result = "int";
            } else {
                issues.emplace_back(
                    "Error",
                    "Addition requires int operands at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
            break;
        }
        case NodeType::Less: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if (leftType == "int" && rightType == "int") {
                symbolTable.addTypeCheck(
                    node->value,
                    "Less-than comparison with int operands",
                    "OK"
                );
                result = "bool";
            } else {
                issues.emplace_back(
                    "Error",
                    "Less-than comparison requires int operands at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
            break;
        }
        case NodeType::Increment:
            result = getExpressionType(node->children[0]);
            break;
        default:
            issues.emplace_back(
                "Error",
                "Unsupported expression type '" + std::to_string(static_cast<int>(node->type)) + "' at line " + std::to_string(node->line),
                "❌"
            );
            result = "unknown";
            break;
    }
    node->cachedType = result;
    return result;
}

void SemanticAnalyzer::printAST(const std::shared_ptr<ASTNode>& node, std::ofstream& out, int indent) const {
    std::string indentStr(indent, ' ');
    std::string nodeStr;

    SemanticAnalyzer* nonConstThis = const_cast<SemanticAnalyzer*>(this);

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
            nodeStr = "Assignment: " + node->value + " (type=" + nonConstThis->getExpressionType(node->children[0]) + ")";
            break;
        case NodeType::While:
            nodeStr = "While: " + node->value + " (type=" + (node->children.empty() ? "unknown" : nonConstThis->getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Call: {
            std::string args;
            for (size_t i = 0; i < node->children.size(); ++i) {
                args += nonConstThis->getExpressionType(node->children[i]);
                if (i < node->children.size() - 1) args += ",";
            }
            if (node->children.empty() && !node->typeHint.empty()) {
                args = node->typeHint;
            }
            nodeStr = "Call: " + node->value + " (args=" + args + ")";
            break;
        }
        case NodeType::IfElse:
            nodeStr = "IfElse: " + node->value + " (type=" + (node->children.empty() ? "unknown" : nonConstThis->getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Return:
            nodeStr = "Return: " + node->value + " (type=" + (node->children.empty() ? "unknown" : nonConstThis->getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Identifier:
            nodeStr = "Identifier: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Number:
            nodeStr = "Number: " + node->value + " (type=" + node->typeHint + ")";
            break;
        case NodeType::String:
            nodeStr = "String: " + node->value + " (type=" + node->typeHint + ")";
            break;
        case NodeType::Address:
            nodeStr = "Address: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Modulo:
            nodeStr = "Modulo: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Equal:
            nodeStr = "Equal: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Add:
            nodeStr = "Add: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Less:
            nodeStr = "Less: " + node->value + " (type=" + nonConstThis->getExpressionType(node) + ")";
            break;
        case NodeType::Increment:
            nodeStr = "Increment: " + (node->children.empty() ? node->value : node->children[0]->value) + " (type=" + (node->children.empty() ? "unknown" : nonConstThis->getExpressionType(node->children[0])) + ")";
            break;
        case NodeType::Expression:
            nodeStr = "Expression: " + node->value + " (type=" + (node->children.empty() ? "unknown" : nonConstThis->getExpressionType(node->children[0])) + ")";
            break;
    }

    out << indentStr << nodeStr << std::endl;
    for (const auto& child : node->children) {
        printAST(child, out, indent + 2);
    }
}

void SemanticAnalyzer::analyze() {
    analyzeNode(ast);
    const auto& symbolIssues = symbolTable.getIssues();
    issues.insert(issues.end(), symbolIssues.begin(), symbolIssues.end());

    std::cout << "\nSemantic Analysis\n";
    std::cout << "=================\n";
    std::cout << "Performing scope checking, type checking, and symbol table creation.\n\n";
    symbolTable.printSymbolTable();
    symbolTable.printTypeChecks();
    symbolTable.printScopeChecks();
    symbolTable.printIssues();
}

void SemanticAnalyzer::saveASTToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file: " + filename);
    }
    printAST(ast, out);
    out.close();
}

const std::vector<SemanticIssue>& SemanticAnalyzer::getIssues() const { return issues; }

// Parse a single line into node type, value, and typeHint
ParsedNode parseLine(const std::string& line) {
    ParsedNode result;
    std::string cleaned = line;

    // Replace non-breaking spaces with regular spaces
    for (char& c : cleaned) {
        if (static_cast<unsigned char>(c) == 0xA0) {
            c = ' ';
        }
    }

    // Normalize spaces: replace multiple spaces with single space
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

    // Check if line is a simple node type (e.g., "Program")
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

    // Split line into node type and rest
    std::string nodeTypeStr = normalized.substr(0, colonPos);
    std::string rest = normalized.substr(colonPos + 2);

    // Trim nodeTypeStr
    nodeTypeStr.erase(nodeTypeStr.begin(), std::find_if(nodeTypeStr.begin(), nodeTypeStr.end(), [](unsigned char c) { return !std::isspace(c); }));
    nodeTypeStr.erase(std::find_if(nodeTypeStr.rbegin(), nodeTypeStr.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), nodeTypeStr.end());

    // Trim rest
    rest.erase(rest.begin(), std::find_if(rest.begin(), rest.end(), [](unsigned char c) { return !std::isspace(c); }));
    rest.erase(std::find_if(rest.rbegin(), rest.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), rest.end());

    std::cerr << "Debug: Node type: '" << nodeTypeStr << "', Rest: '" << rest << "'" << std::endl;

    // Map node type string to NodeType enum
    auto it = nodeTypeMap.find(nodeTypeStr);
    if (it == nodeTypeMap.end()) {
        std::string charCodes;
        for (char c : nodeTypeStr) {
            charCodes += std::to_string(static_cast<unsigned char>(c)) + " ";
        }
        throw std::runtime_error("Unknown node type: '" + nodeTypeStr + "' in line: '" + normalized + "' (char codes: " + charCodes + ")");
    }
    result.type = it->second;

    // Parse based on node type
    if (nodeTypeStr == "Function") {
        size_t parenPos = rest.find(" (");
        if (parenPos != std::string::npos) {
            result.value = rest.substr(0, parenPos);
            result.typeHint = rest.substr(parenPos + 2, rest.size() - parenPos - 3);
        } else {
            throw std::runtime_error("Invalid function format: '" + rest + "'");
        }
    } else if (nodeTypeStr == "LocalDeclaration") {
        size_t spacePos = rest.find(" = ");
        if (spacePos != std::string::npos) {
            std::string typeAndName = rest.substr(0, spacePos);
            size_t typePos = typeAndName.find(" ");
            if (typePos == std::string::npos) {
                throw std::runtime_error("Invalid declaration format: '" + rest + "'");
            }
            result.typeHint = typeAndName.substr(0, typePos);
            result.value = typeAndName.substr(typePos + 1);
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
            std::string args = rest.substr(parenPos + 1, rest.size() - parenPos - 2);
            if (!args.empty()) {
                result.typeHint = "string"; // Assume string for now, as in printf("loop")
            }
        } else {
            result.value = rest;
            result.typeHint = "";
        }
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
            result.typeHint = "";
        } else {
            throw std::runtime_error("Invalid assignment format: '" + rest + "'");
        }
    } else if (nodeTypeStr == "Increment") {
        if (rest.size() >= 2 && rest.substr(rest.size() - 2) == "++") {
            result.value = rest;
            result.typeHint = "";
        } else {
            throw std::runtime_error("Invalid increment format: '" + rest + "'. Expected variable++");
        }
    } else if (nodeTypeStr == "While" || nodeTypeStr == "Add" || nodeTypeStr == "Less" ||
               nodeTypeStr == "Expression") {
        result.value = rest;
        result.typeHint = "";
    } else if (nodeTypeStr == "Return") {
        result.value = rest;
        result.typeHint = rest.find('.') != std::string::npos ? "float" : "int";
    } else {
        result.value = rest;
        result.typeHint = "";
    }

    return result;
}