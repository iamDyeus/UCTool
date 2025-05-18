#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

// Symbol table entry
struct Symbol {
    std::string type;
    std::string scope;
    std::string attributes;
    bool initialized;
    bool used;
    int line;
    Symbol();
    Symbol(const std::string& t, const std::string& s, const std::string& a, int l);
};

// Type checking entry
struct TypeCheck {
    std::string location;
    std::string description;
    std::string status;
    TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat);
};

// Scope checking entry
struct ScopeCheck {
    std::string scope;
    std::string action;
    int symbolCount;
    ScopeCheck(const std::string& s, const std::string& a, int count);
};

// Semantic error/warning entry
struct SemanticIssue {
    std::string type;
    std::string description;
    std::string status;
    SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat);
};

// Symbol table with scope management
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<std::string> scopeNames;
    std::vector<SemanticIssue> issues;
    std::unordered_map<std::string, Symbol> macros;
    std::unordered_map<std::string, Symbol> structs;
    std::unordered_map<std::string, Symbol> functions;
    mutable std::vector<TypeCheck> typeChecks;
    mutable std::vector<ScopeCheck> scopeChecks;
    bool hasStdioInclude;

public:
    SymbolTable();
    void enterScope(const std::string& scopeName);
    void exitScope();
    bool declare(const std::string& name, const std::string& type, const std::string& attributes, int line);
    bool defineMacro(const std::string& name, const std::string& value, int line);
    bool defineStruct(const std::string& name, int line);
    bool defineFunction(const std::string& name, const std::string& type, int line);
    const Symbol* lookup(const std::string& name, int line) const;
    void markUsed(const std::string& name);
    void setStdioInclude();
    bool hasStdio() const;
    void addTypeCheck(const std::string& location, const std::string& description, const std::string& status) const;
    void addWarning(const std::string& description, int line);
    void checkUnusedSymbols();
    void printSymbolTable() const;
    void printTypeChecks() const;
    void printScopeChecks() const;
    void printIssues() const;
    const std::vector<SemanticIssue>& getIssues() const;
    bool hasOnlyWarnings() const;
};