#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

struct Symbol {
    std::string type;
    std::string scope;
    std::string attributes;
    bool initialized;
    bool used;
    int line;
    std::vector<std::string> paramTypes;  // For function parameters
    std::string returnType;               // For function return type
    std::string initialValue;             // For initialized variables
    bool isFunction;                      // To distinguish between variables and functions

    Symbol();
    Symbol(const std::string& t, const std::string& s, const std::string& a, int l);
    Symbol(const std::string& t, const std::string& s, const std::string& a, int l,
           const std::vector<std::string>& params, const std::string& ret);
};

struct TypeCheck {
    std::string location;
    std::string description;
    std::string status;

    TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat);
};

struct ScopeCheck {
    std::string scope;
    std::string action;
    int symbolCount;

    ScopeCheck(const std::string& s, const std::string& a, int count);
};

struct SemanticIssue {
    std::string type;
    std::string description;
    std::string status;

    SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat);
};

class SymbolTable {
private:
    std::vector<std::map<std::string, Symbol>> scopes;  // Stack of scopes for variables
    std::vector<std::string> scopeNames;
    std::map<std::string, Symbol> macros;
    std::map<std::string, Symbol> structs;
    std::map<std::string, Symbol> functions;
    std::vector<TypeCheck> typeChecks;
    std::vector<ScopeCheck> scopeChecks;
    std::vector<SemanticIssue> issues;
    bool hasStdioInclude;
    std::string currentFunction;

public:
    SymbolTable();
    void enterScope(const std::string& scopeName);
    void exitScope();
    bool declare(const std::string& name, const std::string& type, const std::string& attributes, int line);
    bool declareWithInit(const std::string& name, const std::string& type, const std::string& value, int line);
    bool defineMacro(const std::string& name, const std::string& value, int line);
    bool defineStruct(const std::string& name, int line);
    bool defineFunction(const std::string& name, const std::string& type, const std::vector<std::string>& params, int line);
    const Symbol* lookup(const std::string& name, int line) const;
    void markUsed(const std::string& name);
    void setStdioInclude();
    bool hasStdio() const;
    void addTypeCheck(const std::string& location, const std::string& description, const std::string& status);
    void addWarning(const std::string& description, int line);
    void checkUnusedSymbols();
    bool validateFunctionCall(const std::string& name, const std::vector<std::string>& argTypes, int line);
    bool validateReturnType(const std::string& type, int line);
    void enterFunction(const std::string& name);
    void exitFunction();
    void validateDeclaration(const std::string& name, const std::string& type, int line);
    void printSymbolTable() const;
    void printTypeChecks() const;
    void printScopeChecks() const;
    void printIssues() const;
    const std::vector<SemanticIssue>& getIssues() const;
    bool hasOnlyWarnings() const;
};

#endif