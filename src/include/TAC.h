#pragma once
#include <string>
#include <iostream>

// TAC instruction with register support
struct TACInstruction {
    std::string label;
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    int line;
    bool isRegister;
    TACInstruction(const std::string& l, const std::string& o, const std::string& a1, const std::string& a2, const std::string& r, int ln, bool reg = false);
};

// Overload operator<< for TACInstruction
std::ostream& operator<<(std::ostream& os, const TACInstruction& inst);