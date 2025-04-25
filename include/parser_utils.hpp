#include <string>
#include <vector>

struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode() = default;
    ASTNode(const std::string& t, const std::string& v) : type(t), value(v) {}
    ASTNode(const ASTNode& other) : type(other.type), value(other.value) {
        for (const auto* child : other.children) {
            children.push_back(new ASTNode(*child));
        }
    }
    ~ASTNode() {
        for (auto* child : children) {
            delete child;
        }
    }
    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + type;
        if (!value.empty()) result += ": " + value;
        result += "\n";
        for (const auto* child : children) {
            if (child) result += child->to_string(indent + 2);
        }
        return result;
    }
};

struct StatementNode {
    std::string type;
    std::string value;
    std::vector<StatementNode*> statements;
    std::vector<ASTNode*> children;

    StatementNode() = default;
    ~StatementNode() {
        for (auto* stmt : statements) {
            delete stmt;
        }
        for (auto* child : children) {
            delete child;
        }
    }
    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + type;
        if (!value.empty()) result += ": " + value;
        result += "\n";
        for (const auto* child : children) {
            if (child) result += child->to_string(indent + 2);
        }
        for (const auto* stmt : statements) {
            if (stmt) result += stmt->to_string(indent + 2);
        }
        return result;
    }
};

struct FunctionNode {
    std::string return_type;
    std::string name;
    std::vector<StatementNode*> statements;

    ~FunctionNode() {
        // Do not delete statements; managed by parser.y or ProgramNode
    }
    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + "Function: " + name + " (" + return_type + ")\n";
        for (const auto* stmt : statements) {
            if (stmt) result += stmt->to_string(indent + 2);
        }
        return result;
    }
};

struct ProgramNode {
    std::vector<FunctionNode*> functions;
    std::vector<ASTNode*> children;

    ~ProgramNode() {
        for (auto* func : functions) {
            delete func;
        }
        for (auto* child : children) {
            delete child;
        }
    }
    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + "Program\n";
        for (const auto* child : children) {
            if (child) result += child->to_string(indent + 2);
        }
        for (const auto* func : functions) {
            if (func) result += func->to_string(indent + 2);
        }
        return result;
    }
};