#ifndef PARSER_UTILS_HPP
#define PARSER_UTILS_HPP

#include <string>
#include <vector>

class ASTNode {
public:
    std::string type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& t = "", const std::string& v = "") : type(t), value(v) {}
    ~ASTNode() { for (auto* child : children) delete child; }

    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + type;
        if (!value.empty()) {
            // Check for strings containing newlines
            std::string display_value = value;
            if (value.find('\n') != std::string::npos) {
                // Remove quotes and replace newline with \n
                if (display_value.front() == '"' && display_value.back() == '"') {
                    display_value = display_value.substr(1, display_value.length() - 2);
                }
                size_t pos = 0;
                while ((pos = display_value.find('\n', pos)) != std::string::npos) {
                    display_value.replace(pos, 1, "\\n");
                    pos += 2;
                }
                result += ": \"" + display_value + "\"";
            } else if (value == "\"\\n\"") {
                result += ": \"\\n\"";
            } else {
                result += ": " + value;
            }
        }
        result += "\n";
        for (const auto* child : children) {
            result += child->to_string(indent + 2);
        }
        return result;
    }
};

class StatementNode {
public:
    std::string type;
    std::string value;
    std::vector<StatementNode*> statements;
    std::vector<ASTNode*> children;

    StatementNode() : type("Statement") {}
    ~StatementNode() {
        for (auto* stmt : statements) delete stmt;
        for (auto* child : children) delete child;
    }

    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + type;
        if (!value.empty()) {
            // Check for strings containing newlines
            std::string display_value = value;
            if (value.find('\n') != std::string::npos) {
                // Remove quotes and replace newline with \n
                if (display_value.front() == '"' && display_value.back() == '"') {
                    display_value = display_value.substr(1, display_value.length() - 2);
                }
                size_t pos = 0;
                while ((pos = display_value.find('\n', pos)) != std::string::npos) {
                    display_value.replace(pos, 1, "\\n");
                    pos += 2;
                }
                result += ": \"" + display_value + "\"";
            } else if (value == "\"\\n\"") {
                result += ": \"\\n\"";
            } else {
                // Handle Call nodes specifically
                if (type == "Call" && !children.empty()) {
                    // Extract function name from value (before parentheses)
                    size_t paren_pos = value.find('(');
                    std::string func_name = (paren_pos != std::string::npos) ? value.substr(0, paren_pos) : value;
                    result = std::string(indent, ' ') + type + ": " + func_name;
                    // Reconstruct arguments without quotes around the entire call
                    if (paren_pos != std::string::npos) {
                        std::string args = value.substr(paren_pos);
                        size_t pos = 0;
                        while ((pos = args.find('\n', pos)) != std::string::npos) {
                            args.replace(pos, 1, "\\n");
                            pos += 2;
                        }
                        result += args;
                    }
                } else {
                    result += ": " + value;
                }
            }
        }
        result += "\n";
        for (const auto* stmt : statements) {
            result += stmt->to_string(indent + 2);
        }
        for (const auto* child : children) {
            result += child->to_string(indent + 2);
        }
        return result;
    }
};

class FunctionNode {
public:
    std::string return_type;
    std::string name;
    std::vector<StatementNode*> statements;

    FunctionNode() : return_type("void") {}
    ~FunctionNode() { for (auto* stmt : statements) delete stmt; }

    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + "Function: " + name + " (" + return_type + ")\n";
        for (const auto* stmt : statements) {
            result += stmt->to_string(indent + 2);
        }
        return result;
    }
};

class ProgramNode {
public:
    std::vector<FunctionNode*> functions;
    std::vector<ASTNode*> children;

    ~ProgramNode() {
        for (auto* func : functions) delete func;
        for (auto* child : children) delete child;
    }

    std::string to_string(int indent = 0) const {
        std::string result = std::string(indent, ' ') + "Program\n";
        for (const auto* func : functions) {
            result += func->to_string(indent + 2);
        }
        for (const auto* child : children) {
            result += child->to_string(indent + 2);
        }
        return result;
    }
};

#endif // PARSER_UTILS_HPP