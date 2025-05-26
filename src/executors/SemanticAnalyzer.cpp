#include "../include/SemanticAnalyzer.h"
#include "../include/SymbolTable.h"
#include "../include/TAC.h"
#include "../include/DAG.h"
#include "../include/AST.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cstring>

SemanticAnalyzer::SemanticAnalyzer(std::shared_ptr<ASTNode> a) : ast(a), tempCounter(1), labelCounter(1), dagNodeCounter(1), registerCounter(0) {
    registers = {"r1", "r2", "r3", "r4"};
    functionSignatures = {
        {"printf", {{"string"}, {"string", "int"}, {"string", "float"}}},
        {"scanf", {{"string", "int*"}}}
    };
}

std::string SemanticAnalyzer::newTemp() {
    return "t" + std::to_string(tempCounter++);
}

std::string SemanticAnalyzer::newLabel() {
    return "L" + std::to_string(labelCounter++);
}

std::string SemanticAnalyzer::allocateRegister() {
    if (registerCounter < registers.size()) {
        return registers[registerCounter++];
    }
    return newTemp();
}

void SemanticAnalyzer::freeRegister(const std::string& reg) {
    auto it = std::find(registers.begin(), registers.end(), reg);
    if (it != registers.end()) {
        registerCounter--;
        std::swap(*it, registers[registerCounter]);
    }
}

std::shared_ptr<DAGNode> SemanticAnalyzer::findDAGNode(const std::string& op, const std::string& arg1, const std::string& arg2) {
    for (const auto& node : dagNodes) {
        if (node->op == op && node->children.size() == 2 &&
            node->children[0]->value == arg1 && node->children[1]->value == arg2) {
            return node;
        }
    }
    return nullptr;
}

std::shared_ptr<DAGNode> SemanticAnalyzer::createDAGNode(const std::string& op, const std::string& value,
                                                        const std::vector<std::string>& args, int line) {
    std::shared_ptr<DAGNode> node = std::make_shared<DAGNode>(op, value, dagNodeCounter++);
    for (const auto& arg : args) {
        bool found = false;
        for (const auto& existing : dagNodes) {
            if (existing->value == arg && existing->op.empty()) {
                node->children.push_back(existing);
                found = true;
                break;
            }
        }
        if (!found) {
            auto leaf = std::make_shared<DAGNode>("", arg, dagNodeCounter++);
            node->children.push_back(leaf);
            dagNodes.push_back(leaf);
        }
    }
    dagNodes.push_back(node);
    return node;
}

void SemanticAnalyzer::analyzeNode(const std::shared_ptr<ASTNode>& node) {
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
                size_t spacePos = node->value.find(" ", 8);
                if (spacePos == std::string::npos) {
                    issues.emplace_back("Error", "Invalid macro definition at line " + std::to_string(node->line), "❌");
                    break;
                }
                std::string macroName = node->value.substr(8, spacePos - 8);
                std::string macroValue = node->value.substr(spacePos + 1);
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
            analyzeFunction(node);
            break;
        }

        case NodeType::Declarations: {
            for (const auto& child : node->children) {
                if (child->type == NodeType::VarDecl) {
                    analyzeVarDecl(child);
                } else {
                    issues.emplace_back("Error", "Invalid child node in Declarations at line " + std::to_string(child->line), "❌");
                }
            }
            break;
        }

        case NodeType::LocalDeclaration: {
            if (node->value == "declarations") {
                for (const auto& child : node->children) {
                    analyzeNode(child);
                }
            } else {
                analyzeNode(node);
            }
            break;
        }

        case NodeType::VarDecl: {
            analyzeVarDecl(node);
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
            if (exprType == symbol->type || (exprType == "int" && symbol->type == "float")) {
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
            analyzeForLoop(node);
            break;
        }

        case NodeType::Call: {
            std::string funcName = node->value;
            auto it = functionSignatures.find(funcName);
            if (it == functionSignatures.end()) {
                auto symbol = symbolTable.lookup(funcName, node->line);
                if (!symbol || symbol->attributes != "function") {
                    issues.emplace_back(
                        "Error",
                        "Unknown function '" + funcName + "' at line " + std::to_string(node->line),
                        "❌"
                    );
                    break;
                }
                symbolTable.markUsed(funcName);
                symbolTable.addTypeCheck(
                    funcName + "()",
                    "Calling user-defined function " + funcName,
                    "OK"
                );
            } else {
                symbolTable.markUsed(funcName);
                bool valid = false;
                for (const auto& expectedTypes : it->second) {
                    if (node->children.size() == expectedTypes.size()) {
                        bool argsMatch = true;
                        for (size_t i = 0; i < node->children.size(); ++i) {
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
                            if (actualType != expectedTypes[i] && !(actualType == "int" && expectedTypes[i] == "float")) {
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
                                funcName + "(" + (node->children.empty() ? "" : node->children[0]->value) + ")",
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
            }
            break;
        }

        case NodeType::IfElse: {
            if (node->children.size() != 3) {
                issues.emplace_back(
                    "Error",
                    "IfElse node must have exactly 3 children at line " + std::to_string(node->line),
                    "❌"
                );
                break;
            }
            auto condition = node->children[2];
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
            analyzeNode(node->children[0]);
            analyzeNode(node->children[1]);
            break;
        }

        case NodeType::Return: {
            std::string returnType = getExpressionType(node->children[0]);
            if (returnType == node->typeHint || (returnType == "int" && node->typeHint == "float")) {
                symbolTable.addTypeCheck(
                    "return " + node->children[0]->value,
                    "Returning " + returnType + " from function expecting " + node->typeHint,
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Return type mismatch at line " + std::to_string(node->line) + ". Expected " + node->typeHint + ", got " + returnType,
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
            if (symbol->type == "int" || symbol->type == "float") {
                symbolTable.addTypeCheck(
                    identifierNode->value + "++",
                    "Post-increment of " + symbol->type,
                    "OK"
                );
                node->cachedType = symbol->type;
                identifierNode->cachedType = symbol->type;
            } else {
                issues.emplace_back(
                    "Error",
                    "Increment requires int or float operand at line " + std::to_string(node->line) + ". Got " + symbol->type,
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
                node->cachedType = symbol->type;
                symbolTable.addTypeCheck(
                    node->value,
                    "Using variable '" + node->value + "' of type " + symbol->type,
                    "OK"
                );
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

std::string SemanticAnalyzer::getExpressionType(const std::shared_ptr<ASTNode>& node) {
    if (!node->cachedType.empty()) {
        return node->cachedType;
    }

    std::string result;
    switch (node->type) {
        case NodeType::Identifier: {
            auto symbol = symbolTable.lookup(node->value, node->line);
            if (!symbol) {
                issues.emplace_back(
                    "Error",
                    "Undeclared variable '" + node->value + "' at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            } else {
                result = symbol->type;
                node->cachedType = result;
                symbolTable.addTypeCheck(
                    node->value,
                    "Expression uses variable '" + node->value + "' of type " + result,
                    "OK"
                );
            }
            break;
        }
        case NodeType::Number: {
            try {
                std::size_t pos;
                std::stod(node->value, &pos);
                if (node->value.find('.') != std::string::npos && pos == node->value.length()) {
                    result = "float";
                } else {
                    result = "int";
                }
                node->cachedType = result;
                symbolTable.addTypeCheck(
                    node->value,
                    "Number literal '" + node->value + "' of type " + result,
                    "OK"
                );
            } catch (...) {
                issues.emplace_back(
                    "Error",
                    "Invalid number format '" + node->value + "' at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
            break;
        }
        case NodeType::String:
            result = "string";
            node->cachedType = result;
            symbolTable.addTypeCheck(
                node->value,
                "String literal of type " + result,
                "OK"
            );
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
                symbolTable.addTypeCheck(
                    "&" + node->children[0]->value,
                    "Address-of operation yielding " + result,
                    "OK"
                );
            }
            node->cachedType = result;
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
            node->cachedType = result;
            break;
        }
        case NodeType::Equal: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if ((leftType == rightType && leftType != "unknown") ||
                (leftType == "int" && rightType == "float") ||
                (leftType == "float" && rightType == "int")) {
                symbolTable.addTypeCheck(
                    node->value,
                    "Equality comparison with compatible types (" + leftType + ", " + rightType + ")",
                    "OK"
                );
                result = "bool";
            } else {
                issues.emplace_back(
                    "Error",
                    "Type mismatch in comparison at line " + std::to_string(node->line) + ". Got " + leftType + " and " + rightType,
                    "❌"
                );
                result = "unknown";
            }
            node->cachedType = result;
            break;
        }
        case NodeType::Add: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if ((leftType == "int" && rightType == "int") ||
                (leftType == "float" && rightType == "float") ||
                (leftType == "int" && rightType == "float") ||
                (leftType == "float" && rightType == "int")) {
                result = (leftType == "float" || rightType == "float") ? "float" : "int";
                symbolTable.addTypeCheck(
                    node->value,
                    "Addition with compatible operands (" + leftType + ", " + rightType + ") yielding " + result,
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Addition requires int or float operands at line " + std::to_string(node->line) + ". Got " + leftType + " and " + rightType,
                    "❌"
                );
                result = "unknown";
            }
            node->cachedType = result;
            break;
        }
        case NodeType::Less: {
            auto leftType = getExpressionType(node->children[0]);
            auto rightType = getExpressionType(node->children[1]);
            if ((leftType == "int" && rightType == "int") ||
                (leftType == "float" && rightType == "float") ||
                (leftType == "int" && rightType == "float") ||
                (leftType == "float" && rightType == "int")) {
                symbolTable.addTypeCheck(
                    node->value,
                    "Less-than comparison with compatible operands (" + leftType + ", " + rightType + ")",
                    "OK"
                );
                result = "bool";
            } else {
                issues.emplace_back(
                    "Error",
                    "Less-than comparison requires int or float operands at line " + std::to_string(node->line) + ". Got " + leftType + " and " + rightType,
                    "❌"
                );
                result = "unknown";
            }
            node->cachedType = result;
            break;
        }
        case NodeType::Increment:
            result = getExpressionType(node->children[0]);
            node->cachedType = result;
            break;
        default:
            issues.emplace_back(
                "Error",
                "Unsupported expression type '" + std::to_string(static_cast<int>(node->type)) + "' at line " + std::to_string(node->line),
                "❌"
            );
            result = "unknown";
            node->cachedType = result;
            break;
    }
    return result;
}

std::string SemanticAnalyzer::generateExpressionTAC(const std::shared_ptr<ASTNode>& node) {
    switch (node->type) {
        case NodeType::Identifier: {
            if (!node->cachedType.empty()) {
                std::string reg = allocateRegister();
                tacInstructions.emplace_back("", "LOAD", node->value, "", reg, node->line);
                return reg;
            }
            return node->value;
        }
        case NodeType::Number:
        case NodeType::String: {
            std::string reg = allocateRegister();
            std::string value = (node->type == NodeType::String) ? "\"" + node->value + "\"" : node->value;
            tacInstructions.emplace_back("", "LOAD", value, "", reg, node->line);
            return reg;
        }
        case NodeType::Address: {
            std::string reg = allocateRegister();
            std::string value = "&" + node->children[0]->value;
            tacInstructions.emplace_back("", "LOAD", value, "", reg, node->line);
            return reg;
        }
        case NodeType::Modulo: {
            std::string left = generateExpressionTAC(node->children[0]);
            std::string right = generateExpressionTAC(node->children[1]);
            auto existing = findDAGNode("MOD", left, right);
            if (existing && !existing->result.empty()) {
                return existing->result;
            }
            std::string resultReg = allocateRegister();
            auto dagNode = createDAGNode("MOD", "", {left, right}, node->line);
            dagNode->result = resultReg;
            tacInstructions.emplace_back("", "MOD", left, right, resultReg, node->line);
            freeRegister(left);
            freeRegister(right);
            return resultReg;
        }
        case NodeType::Equal: {
            std::string left = generateExpressionTAC(node->children[0]);
            std::string right = generateExpressionTAC(node->children[1]);
            auto existing = findDAGNode("EQ", left, right);
            if (existing && !existing->result.empty()) {
                return existing->result;
            }
            std::string resultReg = allocateRegister();
            auto dagNode = createDAGNode("EQ", "", {left, right}, node->line);
            dagNode->result = resultReg;
            tacInstructions.emplace_back("", "EQ", left, right, resultReg, node->line);
            freeRegister(left);
            freeRegister(right);
            return resultReg;
        }
        case NodeType::Add: {
            std::string left = generateExpressionTAC(node->children[0]);
            std::string right = generateExpressionTAC(node->children[1]);
            auto existing = findDAGNode("ADD", left, right);
            if (existing && !existing->result.empty()) {
                return existing->result;
            }
            std::string resultReg = allocateRegister();
            auto dagNode = createDAGNode("ADD", "", {left, right}, node->line);
            dagNode->result = resultReg;
            tacInstructions.emplace_back("", "ADD", left, right, resultReg, node->line);
            freeRegister(left);
            freeRegister(right);
            return resultReg;
        }
        case NodeType::Less: {
            std::string left = generateExpressionTAC(node->children[0]);
            std::string right = generateExpressionTAC(node->children[1]);
            auto existing = findDAGNode("LT", left, right);
            if (existing && !existing->result.empty()) {
                return existing->result;
            }
            std::string resultReg = allocateRegister();
            auto dagNode = createDAGNode("LT", "", {left, right}, node->line);
            dagNode->result = resultReg;
            tacInstructions.emplace_back("", "LT", left, right, resultReg, node->line);
            freeRegister(left);
            freeRegister(right);
            return resultReg;
        }
        default:
            issues.emplace_back(
                "Error",
                "Unsupported expression type for TAC generation at line " + std::to_string(node->line),
                "❌"
            );
            return "";
    }
}

void SemanticAnalyzer::generateTAC(const std::shared_ptr<ASTNode>& node) {
    switch (node->type) {
        case NodeType::Program:
            for (const auto& child : node->children) {
                generateTAC(child);
            }
            break;

        case NodeType::Function: {
            std::string funcLabel = "func_" + node->value;
            tacInstructions.emplace_back(funcLabel + ":", "", "", "", "", node->line);
            registerCounter = 0;
            dagNodes.clear();
            for (const auto& child : node->children) {
                generateTAC(child);
            }
            tacInstructions.emplace_back("", "END", "", "", "", node->line);
            break;
        }

        case NodeType::VarDecl: {
            if (!node->children.empty()) {
                std::string value = generateExpressionTAC(node->children[0]);
                tacInstructions.emplace_back("", "STORE", value, "", node->value, node->line);
            }
            break;
        }

        case NodeType::Assignment: {
            std::string value = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "STORE", value, "", node->value, node->line);
            break;
        }

        case NodeType::While: {
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            tacInstructions.emplace_back(startLabel + ":", "", "", "", "", node->line);
            std::string cond = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "JZ", cond, "", endLabel, node->line);
            for (size_t i = 1; i < node->children.size(); ++i) {
                generateTAC(node->children[i]);
            }
            tacInstructions.emplace_back("", "JMP", "", "", startLabel, node->line);
            tacInstructions.emplace_back(endLabel + ":", "", "", "", "", node->line);
            break;
        }

        case NodeType::Call: {
            std::string funcName = node->value;
            std::vector<std::string> argRegs;
            for (const auto& child : node->children) {
                std::string arg = generateExpressionTAC(child);
                argRegs.push_back(arg);
            }
            std::string args;
            if (!argRegs.empty()) {
                args = argRegs[0];
                for (size_t i = 1; i < argRegs.size(); ++i) {
                    args += "," + argRegs[i];
                }
            }
            tacInstructions.emplace_back("", "CALL", funcName, args, "", node->line);
            break;
        }

        case NodeType::IfElse: {
            std::string elseLabel = newLabel();
            std::string endLabel = newLabel();
            std::string cond = generateExpressionTAC(node->children[2]);
            tacInstructions.emplace_back("", "JZ", cond, "", elseLabel, node->line);
            generateTAC(node->children[0]);
            tacInstructions.emplace_back("", "JMP", "", "", endLabel, node->line);
            tacInstructions.emplace_back(elseLabel + ":", "", "", "", "", node->line);
            generateTAC(node->children[1]);
            tacInstructions.emplace_back(endLabel + ":", "", "", "", "", node->line);
            break;
        }

        case NodeType::Return: {
            std::string value = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "RET", value, "", "", node->line);
            break;
        }

        default:
            for (const auto& child : node->children) {
                generateTAC(child);
            }
            break;
    }
}

void SemanticAnalyzer::printTAC() const {
    std::ofstream tacFile("../temp/sample.tac");
    if (!tacFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/sample.tac: " << strerror(errno) << std::endl;
        return;
    }

    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    auto print = [&](std::ostream& os) {
        os << "\n╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
        os << "║                                       Three Address Code (TAC)                                        ║\n";
        os << "║                              Generated on: " << std::left << std::setw(40) << timestamp << "║\n";
        os << "╠══════════════════════════════════════════════════════════════════════════════════════════════════════╣\n\n";

        // Column headers with better spacing
        os << "╔═══════════════════════╤═══════════════╤═══════════════════════╤═══════════════╤═══════════════╤═════════╗\n";
        os << "║ " << std::left << std::setw(20) << "Label"
           << "│ " << std::setw(14) << "Operation"
           << "│ " << std::setw(20) << "Argument 1"
           << "│ " << std::setw(14) << "Argument 2"
           << "│ " << std::setw(14) << "Result"
           << "│ " << std::setw(8) << "Line" << "║\n";
        os << "╠═══════════════════════╪═══════════════╪═══════════════════════╪═══════════════╪═══════════════╪═════════╣\n";

        // Print instructions with improved formatting
        for (const auto& inst : tacInstructions) {
            std::string label_trunc = inst.label.length() > 19 ? inst.label.substr(0, 16) + "..." : inst.label;
            std::string op_trunc = inst.op.length() > 13 ? inst.op.substr(0, 10) + "..." : inst.op;
            std::string arg1_trunc = inst.arg1.length() > 19 ? inst.arg1.substr(0, 16) + "..." : inst.arg1;
            std::string arg2_trunc = inst.arg2.length() > 13 ? inst.arg2.substr(0, 10) + "..." : inst.arg2;
            std::string result_trunc = inst.result.length() > 13 ? inst.result.substr(0, 10) + "..." : inst.result;

            os << "║ " << std::left << std::setw(20) << label_trunc
               << "│ " << std::setw(14) << op_trunc
               << "│ " << std::setw(20) << arg1_trunc
               << "│ " << std::setw(14) << arg2_trunc
               << "│ " << std::setw(14) << result_trunc
               << "│ " << std::right << std::setw(8) << inst.line << " ║\n";

            // Add separator after function labels for better readability
            if (!inst.label.empty() && inst.label.find("func_") != std::string::npos) {
                os << "╟───────────────────────┼───────────────┼───────────────────────┼───────────────┼───────────────┼─────────╢\n";
            }
        }
        os << "╚═══════════════════════╧═══════════════╧═══════════════════════╧═══════════════╧═══════════════╧═════════╝\n\n";

        // Summary section
        os << "╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
        os << "║ Summary                                                                                               ║\n";
        os << "╟──────────────────────────────────────────────────────────────────────────────────────────────────────╢\n";
        os << "║ Total Instructions: " << std::left << std::setw(71) << tacInstructions.size() << "║\n";
        os << "║ Status: " << std::left << std::setw(82) << "Generation completed successfully" << "║\n";
        os << "╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n";
    };

    print(std::cout);
    print(tacFile);
    tacFile.close();
}

void SemanticAnalyzer::generateTargetCode() {
    std::ofstream asmFile("../temp/sample.asm");
    if (!asmFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/sample.asm: " << strerror(errno) << std::endl;
        return;
    }

    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    auto print = [&](std::ostream& os) {
        os << "; Target Code (x86 Assembly)\n";
        os << "; Generated on: " << timestamp << "\n";
        os << "; " << std::string(60, '=') << "\n\n";
        os << "section .data\n";
        std::vector<std::string> strings;
        int strCounter = 1;
        for (const auto& inst : tacInstructions) {
            if (inst.op == "LOAD" && inst.arg1[0] == '"') {
                std::string str = inst.arg1;
                strings.push_back(str);
                os << "str" << strCounter++ << ": db " << str << ", 0\n";
            }
        }
        os << "\nsection .text\n";
        os << "global _start\n";
        if (symbolTable.hasStdio()) {
            os << "extern printf\n";
            os << "extern scanf\n";
        }
        os << "\n";

        for (const auto& inst : tacInstructions) {
            if (!inst.label.empty()) {
                os << inst.label << "\n";
            } else if (inst.op == "LOAD") {
                os << "    ; Load value\n";
                os << "    MOV " << inst.result << ", " << inst.arg1 << "\n";
            } else if (inst.op == "STORE") {
                os << "    ; Store value\n";
                os << "    MOV [" << inst.result << "], " << inst.arg1 << "\n";
            } else if (inst.op == "ADD") {
                os << "    ; Add operation\n";
                os << "    MOV RAX, " << inst.arg1 << "\n";
                os << "    ADD RAX, " << inst.arg2 << "\n";
                os << "    MOV " << inst.result << ", RAX\n";
            } else if (inst.op == "MOD") {
                os << "    ; Modulo operation\n";
                os << "    MOV RAX, " << inst.arg1 << "\n";
                os << "    XOR RDX, RDX\n";
                os << "    DIV " << inst.arg2 << "\n";
                os << "    MOV " << inst.result << ", RDX\n";
            } else if (inst.op == "EQ") {
                os << "    ; Equality comparison\n";
                os << "    MOV RAX, " << inst.arg1 << "\n";
                os << "    CMP RAX, " << inst.arg2 << "\n";
                os << "    SETE AL\n";
                os << "    MOVZX " << inst.result << ", AL\n";
            } else if (inst.op == "LT") {
                os << "    ; Less-than comparison\n";
                os << "    MOV RAX, " << inst.arg1 << "\n";
                os << "    CMP RAX, " << inst.arg2 << "\n";
                os << "    SETL AL\n";
                os << "    MOVZX " << inst.result << ", AL\n";
            } else if (inst.op == "CALL") {
                os << "    ; Call function\n";
                if (inst.arg1 == "printf") {
                    size_t strIndex = 0;
                    for (size_t i = 0; i < strings.size(); ++i) {
                        if (inst.arg2.find(strings[i]) != std::string::npos) {
                            strIndex = i + 1;
                            break;
                        }
                    }
                    if (strIndex > 0) {
                        os << "    PUSH str" << strIndex << "\n";
                        os << "    CALL printf\n";
                        os << "    ADD RSP, 8\n";
                    } else {
                        os << "    CALL " << inst.arg1 << "\n";
                    }
                } else {
                    os << "    CALL " << inst.arg1 << "\n";
                }
            } else if (inst.op == "JMP") {
                os << "    ; Jump\n";
                os << "    JMP " << inst.result << "\n";
            } else if (inst.op == "JZ") {
                os << "    ; Jump if zero\n";
                os << "    CMP " << inst.arg1 << ", 0\n";
                os << "    JE " << inst.result << "\n";
            } else if (inst.op == "RET") {
                os << "    ; Return\n";
                os << "    MOV RAX, " << inst.arg1 << "\n";
                os << "    RET\n";
            } else if (inst.op == "END") {
                os << "    ; End function\n";
                os << "    RET\n";
            }
        }

        os << "\n_start:\n";
        os << "    ; Program entry point\n";
        os << "    CALL func_main\n";
        os << "    MOV RAX, 60\n";
        os << "    XOR RDI, RDI\n";
        os << "    SYSCALL\n";
        os << "; " << std::string(60, '=') << "\n";
    };

    print(std::cout);
    print(asmFile);
    asmFile.close();
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
        case NodeType::Declarations:
            nodeStr = "Declarations";
            break;
        case NodeType::VarDecl:
            nodeStr = "VarDecl: " + node->typeHint + " " + node->value + " (type=" + node->typeHint + ")";
            break;
        case NodeType::Assignment:
            nodeStr = "Assignment: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        case NodeType::While:
            nodeStr = "While: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        case NodeType::Call: {
            std::string args;
            for (size_t i = 0; i < node->children.size(); ++i) {
                args += node->children[i]->cachedType.empty() ? "unknown" : node->children[i]->cachedType;
                if (i < node->children.size() - 1) args += ",";
            }
            nodeStr = "Call: " + node->callString + " (args=" + args + ")";
            break;
        }
        case NodeType::IfElse:
            nodeStr = "IfElse: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[2]->cachedType) + ")";
            break;
        case NodeType::Return:
            nodeStr = "Return: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        case NodeType::Identifier:
            nodeStr = "Identifier: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Number:
            nodeStr = "Number: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::String:
            nodeStr = "String: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Address:
            nodeStr = "Address: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Modulo:
            nodeStr = "Modulo: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Equal:
            nodeStr = "Equal: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Add:
            nodeStr = "Add: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Less:
            nodeStr = "Less: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Increment:
            nodeStr = "Increment: " + (node->children.empty() ? node->value : node->children[0]->value) + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        default:
            nodeStr = "Unknown: " + node->value;
            break;
    }

    out << indentStr << nodeStr << "\n";
    for (const auto& child : node->children) {
        printAST(child, out, indent + 2);
    }
}

void SemanticAnalyzer::analyzeSemantics() {
    if (ast) {
        analyzeNode(ast);
        symbolTable.printSymbolTable();
        symbolTable.printTypeChecks();
        symbolTable.printScopeChecks();
        symbolTable.printIssues();
        issues = symbolTable.getIssues();
        saveASTToFile("../temp/processed_ast.txt");
        if (symbolTable.hasOnlyWarnings()) {
            std::cout << "Semantic analysis completed with warnings.\n";
        } else if (symbolTable.getIssues().empty()) {
            std::cout << "Semantic analysis completed successfully.\n";
        } else {
            std::cout << "Semantic analysis failed due to errors.\n";
        }
    } else {
        std::cerr << "Error: No AST provided for semantic analysis" << std::endl;
    }
}

void SemanticAnalyzer::generateTACOnly() {
    if (ast) {
        analyzeNode(ast);
        generateTAC(ast);
        printTAC();
        issues = symbolTable.getIssues();
        std::cout << "TAC generation completed successfully.\n";
    } else {
        std::cerr << "Error: No AST provided for TAC generation" << std::endl;
    }
}

void SemanticAnalyzer::generateTargetCodeOnly() {
    if (ast) {
        analyzeNode(ast);
        generateTAC(ast);
        generateTargetCode();
        issues = symbolTable.getIssues();
        std::cout << "Target code generation completed successfully.\n";
    } else {
        std::cerr << "Error: No AST provided for target code generation" << std::endl;
    }
}

void SemanticAnalyzer::saveASTToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: Failed to open " << filename << " for writing" << std::endl;
        return;
    }
    printAST(ast, out);
    out.close();
}

const std::vector<SemanticIssue>& SemanticAnalyzer::getIssues() const {
    return issues;
}

void SemanticAnalyzer::analyzeFunction(const std::shared_ptr<ASTNode>& node) {
    if (node->type != NodeType::Function) return;

    // Extract function name and return type
    std::string funcName = node->value;
    std::string returnType = node->typeHint.empty() ? "void" : node->typeHint;
    
    // Enter function scope
    symbolTable.enterFunction(funcName);
    currentFunctionReturnType = returnType;

    // Collect parameters if any
    std::vector<std::string> paramTypes;
    // TODO: Parse parameters from node->value if present (e.g., "main (int argc, char** argv)")

    // Define function in symbol table
    symbolTable.defineFunction(funcName, returnType, paramTypes, node->line);

    // Analyze function body
    for (const auto& child : node->children) {
        analyzeNode(child);
    }

    // Exit function scope
    symbolTable.exitFunction();
    currentFunctionReturnType = "";
}

void SemanticAnalyzer::analyzeForLoop(const std::shared_ptr<ASTNode>& node) {
    if (node->type != NodeType::For) return;

    // Enter new scope for the loop
    symbolTable.enterScope("for_loop");

    std::string startLabel = newLabel();
    std::string endLabel = newLabel();
    loopLabels.push_back(endLabel);  // For break statements

    // Process initialization
    if (!node->children.empty() && node->children[0]->type == NodeType::Init) {
        analyzeNode(node->children[0]);
    }

    // Generate TAC for loop
    generateForLoopTAC(node);

    // Analyze loop body
    for (const auto& child : node->children) {
        if (child->type != NodeType::Init && 
            child->type != NodeType::Condition && 
            child->type != NodeType::Update) {
            analyzeNode(child);
        }
    }

    // Exit loop scope
    symbolTable.exitScope();
    loopLabels.pop_back();
}

void SemanticAnalyzer::generateForLoopTAC(const std::shared_ptr<ASTNode>& node) {
    std::string startLabel = newLabel();
    std::string updateLabel = newLabel();
    std::string endLabel = newLabel();

    // Initialization
    if (!node->children.empty() && node->children[0]->type == NodeType::Init) {
        generateTAC(node->children[0]);
    }

    // Loop start
    tacInstructions.emplace_back("", "LABEL", "", "", startLabel, node->line);

    // Condition
    std::string condResult;
    for (const auto& child : node->children) {
        if (child->type == NodeType::Condition) {
            condResult = generateExpressionTAC(child);
            break;
        }
    }
    tacInstructions.emplace_back("", "JZ", condResult, "", endLabel, node->line);

    // Loop body
    for (const auto& child : node->children) {
        if (child->type != NodeType::Init && 
            child->type != NodeType::Condition && 
            child->type != NodeType::Update) {
            generateTAC(child);
        }
    }

    // Update
    tacInstructions.emplace_back("", "LABEL", "", "", updateLabel, node->line);
    for (const auto& child : node->children) {
        if (child->type == NodeType::Update) {
            generateTAC(child);
            break;
        }
    }

    // Jump back to condition
    tacInstructions.emplace_back("", "JMP", "", "", startLabel, node->line);
    
    // Loop end
    tacInstructions.emplace_back("", "LABEL", "", "", endLabel, node->line);
}

void SemanticAnalyzer::analyzeVarDecl(const std::shared_ptr<ASTNode>& node) {
    if (node->type != NodeType::VarDecl) return;

    std::string varName = node->value;
    std::string varType = node->typeHint;

    // Check for initialization
    if (!node->children.empty()) {
        std::string initValue = node->children[0]->value;
        symbolTable.declareWithInit(varName, varType, initValue, node->line);
        
        // Generate TAC for initialization
        std::string temp = generateExpressionTAC(node->children[0]);
        tacInstructions.emplace_back("", "STORE", temp, "", varName, node->line);
    } else {
        symbolTable.declare(varName, varType, "", node->line);
    }
}

void SemanticAnalyzer::analyzeCompoundAssign(const std::shared_ptr<ASTNode>& node) {
    if (node->type != NodeType::CompoundAssign) return;

    std::string var = node->value;
    const Symbol* symbol = symbolTable.lookup(var, node->line);
    
    if (!symbol) {
        symbolTable.addWarning("Use of undeclared variable in compound assignment", node->line);
        return;
    }

    // Generate TAC for compound assignment
    generateCompoundAssignTAC(node);
}

void SemanticAnalyzer::generateCompoundAssignTAC(const std::shared_ptr<ASTNode>& node) {
    if (!node || node->type != NodeType::CompoundAssign) return;

    std::string var = node->value;
    std::string op = node->typeHint;  // Assuming typeHint stores the operator type (+=, -=, etc.)
    
    // Generate TAC for the right-hand side expression
    std::string rhs = generateExpressionTAC(node->children[0]);
    
    // Load the current value of the variable
    std::string temp1 = newTemp();
    tacInstructions.emplace_back("", "LOAD", var, "", temp1, node->line);
    
    // Perform the operation
    std::string temp2 = newTemp();
    std::string tacOp;
    
    if (op == "+=") tacOp = "ADD";
    else if (op == "-=") tacOp = "SUB";
    else if (op == "*=") tacOp = "MUL";
    else if (op == "/=") tacOp = "DIV";
    else if (op == "%=") tacOp = "MOD";
    else {
        issues.emplace_back("Error", "Unsupported compound assignment operator: " + op, "❌");
        return;
    }
    
    tacInstructions.emplace_back("", tacOp, temp1, rhs, temp2, node->line);
    
    // Store the result back in the variable
    tacInstructions.emplace_back("", "STORE", temp2, "", var, node->line);
}