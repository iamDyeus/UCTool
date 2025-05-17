#include "SemanticAnalyzer.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

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
                if (initType == node->typeHint || (initType == "int" && node->typeHint == "float")) {
                    symbolTable.addTypeCheck(
                        node->value + " = " + node->children[0]->value,
                        "Assigning " + initType + " to " + node->typeHint + " variable",
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
            if (!node->children.empty()) {
                symbolTable.markUsed(node->value);
            }
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
            std::string returnType = getExpressionType(node->children[0]);
            if (returnType == node->typeHint || (returnType == "int" && node->typeHint == "float")) {
                symbolTable.addTypeCheck(
                    "return " + node->children[0]->value,
                    "Returning " + returnType + " from function",
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
            if ((leftType == rightType && leftType != "unknown") ||
                (leftType == "int" && rightType == "float") ||
                (leftType == "float" && rightType == "int")) {
                symbolTable.addTypeCheck(
                    node->value,
                    "Equality comparison with compatible types",
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
            if ((leftType == "int" && rightType == "int") ||
                (leftType == "float" && rightType == "float") ||
                (leftType == "int" && rightType == "float") ||
                (leftType == "float" && rightType == "int")) {
                result = (leftType == "float" || rightType == "float") ? "float" : "int";
                symbolTable.addTypeCheck(
                    node->value,
                    "Addition with compatible operands",
                    "OK"
                );
            } else {
                issues.emplace_back(
                    "Error",
                    "Addition requires int or float operands at line " + std::to_string(node->line),
                    "❌"
                );
                result = "unknown";
            }
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
                    "Less-than comparison with compatible operands",
                    "OK"
                );
                result = "bool";
            } else {
                issues.emplace_back(
                    "Error",
                    "Less-than comparison requires int or float operands at line " + std::to_string(node->line),
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

std::string SemanticAnalyzer::generateExpressionTAC(const std::shared_ptr<ASTNode>& node) {
    switch (node->type) {
        case NodeType::Identifier: {
            if (!node->cachedType.empty()) {
                std::string reg = allocateRegister();
                tacInstructions.emplace_back("", "LOAD", node->value, "", reg, node->line, true);
                return reg;
            }
            issues.emplace_back(
                "Error",
                "Identifier '" + node->value + "' has no cached type at line " + std::to_string(node->line),
                "❌"
            );
            return node->value;
        }
        case NodeType::Number:
        case NodeType::String: {
            std::string reg = allocateRegister();
            std::string value = (node->type == NodeType::String) ? "\"" + node->value + "\"" : node->value;
            tacInstructions.emplace_back("", "LOAD", value, "", reg, node->line, true);
            return reg;
        }
        case NodeType::Address: {
            std::string reg = allocateRegister();
            std::string value = "&" + node->children[0]->value;
            tacInstructions.emplace_back("", "LOAD", value, "", reg, node->line, true);
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
            tacInstructions.emplace_back("", "MOD", left, right, resultReg, node->line, true);
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
            tacInstructions.emplace_back("", "EQ", left, right, resultReg, node->line, true);
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
            tacInstructions.emplace_back("", "ADD", left, right, resultReg, node->line, true);
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
            tacInstructions.emplace_back("", "LT", left, right, resultReg, node->line, true);
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

        case NodeType::LocalDeclaration: {
            if (!node->children.empty()) {
                std::string value = generateExpressionTAC(node->children[0]);
                tacInstructions.emplace_back("", "STORE", value, "", node->value, node->line);
                freeRegister(value);
            }
            break;
        }

        case NodeType::Assignment: {
            std::string value = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "STORE", value, "", node->value, node->line);
            freeRegister(value);
            break;
        }

        case NodeType::While: {
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            tacInstructions.emplace_back(startLabel + ":", "", "", "", "", node->line);
            std::string cond = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "JZ", cond, "", endLabel, node->children[0]->line);
            freeRegister(cond);
            for (size_t i = 1; i < node->children.size(); ++i) {
                generateTAC(node->children[i]);
            }
            tacInstructions.emplace_back("", "JMP", "", "", startLabel, node->line);
            tacInstructions.emplace_back(endLabel + ":", "", "", "", "", node->line);
            break;
        }

        case NodeType::Call: {
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
            tacInstructions.emplace_back("", "CALL", node->value, args, "", node->line);
            for (const auto& reg : argRegs) {
                freeRegister(reg);
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
                return;
            }
            std::string elseLabel = newLabel();
            std::string endLabel = newLabel();
            std::string cond = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "JZ", cond, "", elseLabel, node->children[0]->line);
            freeRegister(cond);
            generateTAC(node->children[1]); // Then branch
            tacInstructions.emplace_back("", "JMP", "", "", endLabel, node->line);
            tacInstructions.emplace_back(elseLabel + ":", "", "", "", "", node->line);
            generateTAC(node->children[2]); // Else branch
            tacInstructions.emplace_back(endLabel + ":", "", "", "", "", node->line);
            break;
        }

        case NodeType::Increment: {
            std::string var = generateExpressionTAC(node->children[0]);
            std::string temp = allocateRegister();
            tacInstructions.emplace_back("", "ADD", var, "1", temp, node->line, true);
            tacInstructions.emplace_back("", "STORE", temp, "", node->children[0]->value, node->line);
            freeRegister(var);
            freeRegister(temp);
            break;
        }

        case NodeType::Return: {
            std::string value = generateExpressionTAC(node->children[0]);
            tacInstructions.emplace_back("", "RET", value, "", "", node->line);
            freeRegister(value);
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
    std::ofstream tacFile("sample.tac");
    if (!tacFile.is_open()) {
        std::cerr << "Error: Failed to open sample.tac for writing" << std::endl;
        return;
    }

    tacFile << "Three Address Code for sample.ast\n";
    tacFile << "=================================\n\n";
    tacFile << "Instructions\n";
    tacFile << "------------\n\n";
    tacFile << std::left
            << std::setw(15) << "Label"
            << std::setw(11) << "Operation"
            << std::setw(24) << "Arg1"
            << std::setw(12) << "Arg2"
            << std::setw(12) << "Result"
            << std::setw(12) << "Source Line" << "\n";
    tacFile << std::string(80, '-') << "\n";

    for (const auto& inst : tacInstructions) {
        tacFile << std::setw(15) << inst.label
                << std::setw(11) << inst.op
                << std::setw(24) << inst.arg1
                << std::setw(12) << inst.arg2
                << std::setw(12) << inst.result
                << std::setw(12) << inst.line << "\n";
    }

    tacFile << std::string(80, '-') << "\n";
    tacFile << "\n=================================\n";
    tacFile << "TAC generation with registers completed.\n";
    tacFile.close();
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
            nodeStr = "Call: " + node->value + " (args=" + args + ")";
            break;
        }
        case NodeType::IfElse:
            nodeStr = "IfElse: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        case NodeType::Return:
            nodeStr = "Return: " + node->value + " (type=" + (node->children.empty() ? "unknown" : node->children[0]->cachedType) + ")";
            break;
        case NodeType::Identifier:
            nodeStr = "Identifier: " + node->value + " (type=" + (node->cachedType.empty() ? "unknown" : node->cachedType) + ")";
            break;
        case NodeType::Number:
            nodeStr = "Number: " + node->value + " (type=" + node->typeHint + ")";
            break;
        case NodeType::String:
            nodeStr = "String: " + node->value + " (type=" + node->typeHint + ")";
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

void SemanticAnalyzer::analyze() {
    if (ast) {
        analyzeNode(ast);
        generateTAC(ast);
        printTAC();
        symbolTable.printSymbolTable();
        symbolTable.printTypeChecks();
        symbolTable.printScopeChecks();
        symbolTable.printIssues();
        issues = symbolTable.getIssues();
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