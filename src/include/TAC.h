#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>

struct TACInstruction {
    std::string label;
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
    int line;
    std::string comment;

    TACInstruction(std::string l = "", std::string o = "", std::string a1 = "", 
                  std::string a2 = "", std::string r = "", int ln = 0, std::string c = "")
        : label(l), op(o), arg1(a1), arg2(a2), result(r), line(ln), comment(c) {}
};

class TACGenerator {
private:
    std::vector<TACInstruction> instructions;
    int tempCounter;
    int labelCounter;

public:
    TACGenerator();
    std::string newTemp();
    std::string newLabel();
    void addInstruction(std::string op, std::string arg1 = "", std::string arg2 = "", 
                       std::string result = "", int line = 0, std::string comment = "");
    const std::vector<TACInstruction>& getInstructions() const;
    void printInstructions() const;
    void optimizeCode();  // Basic optimization
};

#endif