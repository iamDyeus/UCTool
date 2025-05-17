#include "TAC.h"

TACInstruction::TACInstruction(const std::string& l, const std::string& o, const std::string& a1, const std::string& a2, const std::string& r, int ln, bool reg)
    : label(l), op(o), arg1(a1), arg2(a2), result(r), line(ln), isRegister(reg) {}

std::ostream& operator<<(std::ostream& os, const TACInstruction& inst) {
    if (!inst.label.empty()) {
        os << inst.label;
    } else if (inst.op == "LOAD") {
        os << "LOAD " << inst.arg1 << " -> " << inst.result;
    } else if (inst.op == "STORE") {
        os << "STORE " << inst.arg1 << " -> " << inst.result;
    } else if (inst.op == "ADD" || inst.op == "SUB" || inst.op == "MUL" || inst.op == "DIV") {
        os << inst.op << " " << inst.arg1 << ", " << inst.arg2 << " -> " << inst.result;
    } else if (inst.op == "RET") {
        os << "RET " << inst.arg1;
    } else if (inst.op == "CALL") {
        os << "CALL " << inst.arg1;
        if (!inst.arg2.empty()) os << " with " << inst.arg2;
    } else if (inst.op == "JMP") {
        os << "JMP " << inst.result;
    } else if (inst.op == "JZ") {
        os << "JZ " << inst.arg1 << " -> " << inst.result;
    } else {
        os << inst.op << " " << inst.arg1 << ", " << inst.arg2 << " -> " << inst.result;
    }
    return os;
}