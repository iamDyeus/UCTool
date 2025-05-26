#include "../include/TAC.h"
#include <iostream>
#include <iomanip>

TACGenerator::TACGenerator() : tempCounter(0), labelCounter(0) {}

std::string TACGenerator::newTemp() {
    return "t" + std::to_string(++tempCounter);
}

std::string TACGenerator::newLabel() {
    return "L" + std::to_string(++labelCounter);
}

void TACGenerator::addInstruction(std::string op, std::string arg1, std::string arg2, 
                                std::string result, int line, std::string comment) {
    // Create instruction with empty label by default
    instructions.emplace_back("", op, arg1, arg2, result, line, comment);
}

const std::vector<TACInstruction>& TACGenerator::getInstructions() const {
    return instructions;
}

void TACGenerator::printInstructions() const {
    std::cout << "\nThree Address Code (TAC)\n";
    std::cout << "Generated on: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << std::string(80, '-') << "\n\n";

    std::cout << std::left
              << std::setw(20) << "Label"
              << std::setw(12) << "Op"
              << std::setw(20) << "Arg1"
              << std::setw(12) << "Arg2"
              << std::setw(12) << "Result"
              << std::setw(10) << "Line"
              << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& inst : instructions) {
        std::cout << std::left
                  << std::setw(20) << inst.label
                  << std::setw(12) << inst.op
                  << std::setw(20) << inst.arg1
                  << std::setw(12) << inst.arg2
                  << std::setw(12) << inst.result
                  << std::setw(10) << inst.line;
        
        if (!inst.comment.empty()) {
            std::cout << "  ; " << inst.comment;
        }
        std::cout << "\n";
    }

    std::cout << "\nTotal Instructions: " << instructions.size() << "\n";
    std::cout << std::string(80, '-') << "\n";
}

void TACGenerator::optimizeCode() {
    // Basic optimizations:
    // 1. Remove redundant loads
    // 2. Remove unused temporaries
    // 3. Combine consecutive operations
    // TODO: Implement optimizations
}