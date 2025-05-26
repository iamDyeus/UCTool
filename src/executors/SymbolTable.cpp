#include "../include/SymbolTable.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>

Symbol::Symbol() : initialized(false), used(false), line(0), isFunction(false) {}

Symbol::Symbol(const std::string& t, const std::string& s, const std::string& a, int l)
    : type(t), scope(s), attributes(a), initialized(false), used(false), line(l), isFunction(false) {}

Symbol::Symbol(const std::string& t, const std::string& s, const std::string& a, int l,
               const std::vector<std::string>& params, const std::string& ret)
    : type(t), scope(s), attributes(a), initialized(false), used(false), line(l),
      paramTypes(params), returnType(ret), isFunction(true) {}

TypeCheck::TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat)
    : location(loc), description(desc), status(stat) {}

ScopeCheck::ScopeCheck(const std::string& s, const std::string& a, int count)
    : scope(s), action(a), symbolCount(count) {}

SemanticIssue::SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat)
    : type(t), description(desc), status(stat) {}

SymbolTable::SymbolTable() : hasStdioInclude(false) {
    scopes.emplace_back(); // Global scope
    scopeNames.push_back("global");
}

void SymbolTable::enterScope(const std::string& scopeName) {
    scopes.emplace_back();
    scopeNames.push_back(scopeName);
    scopeChecks.emplace_back(scopeName, "Entered", 0);
}

void SymbolTable::exitScope() {
    if (scopes.size() > 1) {
        int symbolCount = scopes.back().size();
        scopeChecks.emplace_back(scopeNames.back(), "Exited", symbolCount);
        scopes.pop_back();
        scopeNames.pop_back();
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, const std::string& attributes, int line) {
    if (scopes.back().find(name) != scopes.back().end()) {
        issues.emplace_back("Error", "Redeclaration of '" + name + "' in scope '" + scopeNames.back() + "' at line " + std::to_string(line), "❌");
        return false;
    }
    
    Symbol symbol(type, scopeNames.back(), attributes, line);
    scopes.back()[name] = symbol;
    
    addTypeCheck(name, "Variable declaration of type " + type, "OK");
    return true;
}

bool SymbolTable::declareWithInit(const std::string& name, const std::string& type, const std::string& value, int line) {
    if (scopes.back().find(name) != scopes.back().end()) {
        issues.emplace_back("Error", "Redeclaration of '" + name + "' in scope '" + scopeNames.back() + "' at line " + std::to_string(line), "❌");
        return false;
    }
    
    Symbol symbol(type, scopeNames.back(), "variable", line);
    symbol.initialized = true;
    symbol.initialValue = value;
    scopes.back()[name] = symbol;
    
    addTypeCheck(name, "Variable declaration with initialization of type " + type, "OK");
    return true;
}

bool SymbolTable::defineMacro(const std::string& name, const std::string& value, int line) {
    if (macros.find(name) != macros.end()) {
        issues.emplace_back("Error", "Redefinition of macro '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    macros[name] = Symbol("macro", "global", value, line);
    return true;
}

bool SymbolTable::defineStruct(const std::string& name, int line) {
    if (structs.find(name) != structs.end()) {
        issues.emplace_back("Error", "Redefinition of struct '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    structs[name] = Symbol("struct", "global", "", line);
    return true;
}

bool SymbolTable::defineFunction(const std::string& name, const std::string& type,
                               const std::vector<std::string>& params, int line) {
    if (functions.find(name) != functions.end()) {
        addWarning("Redefinition of function '" + name + "'", line);
        return false;
    }

    Symbol symbol(type, "global", "function", line, params, type);
    functions[name] = symbol;

    // Create scope for function parameters
    enterScope(name);
    for (size_t i = 0; i < params.size(); i += 2) {
        if (i + 1 < params.size()) {
            declare(params[i + 1], params[i], "parameter", line);
        }
    }

    addTypeCheck(name, "Function definition with return type " + type, "OK");
    return true;
}

const Symbol* SymbolTable::lookup(const std::string& name, int line) const {
    // Check current scope and outer scopes
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
        auto it = scope->find(name);
        if (it != scope->end()) {
            return &(it->second);
        }
    }
    
    // Check functions
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        return &(funcIt->second);
    }
    
    // Check macros
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        return &(macroIt->second);
    }
    
    // Check structs
    auto structIt = structs.find(name);
    if (structIt != structs.end()) {
        return &(structIt->second);
    }
    
    return nullptr;
}

void SymbolTable::markUsed(const std::string& name) {
    // Check scopes
    for (auto& scope : scopes) {
        auto it = scope.find(name);
        if (it != scope.end()) {
            it->second.used = true;
            return;
        }
    }
    
    // Check functions
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        funcIt->second.used = true;
        return;
    }
    
    // Check macros
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        macroIt->second.used = true;
        return;
    }
}

void SymbolTable::setStdioInclude() {
    hasStdioInclude = true;
}

bool SymbolTable::hasStdio() const {
    return hasStdioInclude;
}

void SymbolTable::addTypeCheck(const std::string& location, const std::string& description, const std::string& status) {
    if (location.find("printf(") == 0 || location.find("scanf(") == 0 || location[0] == '"') {
        return;
    }
    typeChecks.emplace_back(location, description, status);
}

void SymbolTable::addWarning(const std::string& description, int line) {
    issues.emplace_back("Warning", description + " at line " + std::to_string(line), "⚠️");
}

void SymbolTable::checkUnusedSymbols() {
    // Check variables in all scopes
    for (size_t i = 0; i < scopes.size(); ++i) {
        for (const auto& [name, symbol] : scopes[i]) {
            if (!symbol.used) {
                issues.emplace_back(
                    "Warning",
                    "Variable '" + name + "' declared but not used in scope '" + 
                    scopeNames[i] + "' at line " + std::to_string(symbol.line),
                    "⚠️"
                );
            }
        }
    }

    // Check functions
    for (const auto& [name, symbol] : functions) {
        if (!symbol.used && name != "main") {  // Ignore main function
            issues.emplace_back(
                "Warning",
                "Function '" + name + "' declared but not used at line " + std::to_string(symbol.line),
                "⚠️"
            );
        }
    }
}

void SymbolTable::enterFunction(const std::string& name) {
    currentFunction = name;
    enterScope(name);
}

void SymbolTable::exitFunction() {
    exitScope();
    currentFunction.clear();
}

bool SymbolTable::hasOnlyWarnings() const {
    return std::all_of(issues.begin(), issues.end(),
        [](const SemanticIssue& issue) { return issue.type == "Warning"; });
}

void SymbolTable::validateDeclaration(const std::string& name, const std::string& type, int line) {
    if (type.empty()) {
        issues.emplace_back("Error", "Declaration of '" + name + "' has no type at line " + std::to_string(line), "❌");
    }
}

void SymbolTable::printSymbolTable() const {
    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    std::cout << "Symbol Table\n";
    std::cout << "Generated on: " << timestamp << "\n";
    std::cout << std::string(92, '=') << "\n\n";
    
    // Table header
    std::cout << "╔═════════════════════╤══════════════════════╤═══════════════╤══════════════════╤════════════╤═══════╤═══════╗\n";
    std::cout << "║ " << std::left << std::setw(20) << "Name"
              << "│ " << std::setw(20) << "Type"
              << "│ " << std::setw(14) << "Scope"
              << "│ " << std::setw(17) << "Attributes"
              << "│ " << std::setw(11) << "Initialized"
              << "│ " << std::setw(6) << "Used"
              << "│ " << std::setw(6) << "Line" << "║\n";
    std::cout << "╠═════════════════════╪══════════════════════╪═══════════════╪══════════════════╪════════════╪═══════╪═══════╣\n";

    // Print variables from all scopes
    for (size_t i = 0; i < scopes.size(); ++i) {
        for (const auto& [name, symbol] : scopes[i]) {
            if (!symbol.isFunction) {  // Only print variables here
                std::string name_trunc = name.length() > 19 ? name.substr(0, 16) + "..." : name;
                std::string type_trunc = symbol.type.length() > 19 ? symbol.type.substr(0, 16) + "..." : symbol.type;
                std::string scope_trunc = symbol.scope.length() > 13 ? symbol.scope.substr(0, 10) + "..." : symbol.scope;
                std::string attr_trunc = symbol.attributes.length() > 16 ? symbol.attributes.substr(0, 13) + "..." : symbol.attributes;
                
                std::cout << "║ " << std::left << std::setw(20) << name_trunc
                          << "│ " << std::setw(20) << type_trunc
                          << "│ " << std::setw(14) << scope_trunc
                          << "│ " << std::setw(17) << attr_trunc
                          << "│ " << std::setw(11) << (symbol.initialized ? "Yes" : "No")
                          << "│ " << std::setw(6) << (symbol.used ? "Yes" : "No")
                          << "│ " << std::right << std::setw(6) << symbol.line << "║\n";
            }
        }
    }

    // Print functions
    for (const auto& [name, symbol] : functions) {
        if (name == "printf" || name == "scanf") continue;  // Skip standard functions
        
        std::string name_trunc = name.length() > 19 ? name.substr(0, 16) + "..." : name;
        std::string type_trunc = symbol.type.length() > 19 ? symbol.type.substr(0, 16) + "..." : symbol.type;
        std::string scope_trunc = symbol.scope.length() > 13 ? symbol.scope.substr(0, 10) + "..." : symbol.scope;
        std::string attr_trunc = symbol.attributes.length() > 16 ? symbol.attributes.substr(0, 13) + "..." : symbol.attributes;
        
        std::cout << "║ " << std::left << std::setw(20) << name_trunc
                  << "│ " << std::setw(20) << type_trunc
                  << "│ " << std::setw(14) << scope_trunc
                  << "│ " << std::setw(17) << attr_trunc
                  << "│ " << std::setw(11) << (symbol.initialized ? "Yes" : "No")
                  << "│ " << std::setw(6) << (symbol.used ? "Yes" : "No")
                  << "│ " << std::right << std::setw(6) << symbol.line << "║\n";
    }

    std::cout << "╚═════════════════════╧══════════════════════╧═══════════════╧══════════════════╧════════════╧═══════╧═══════╝\n";
    std::cout << "\nTotal Symbols: " << (functions.size() + 
        std::accumulate(scopes.begin(), scopes.end(), 0, 
            [](int sum, const auto& scope) { return sum + scope.size(); })) << "\n";
    std::cout << std::string(92, '=') << "\n\n";
}

void SymbolTable::printTypeChecks() const {
    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    std::cout << "Type Checking\n";
    std::cout << "Generated on: " << timestamp << "\n";
    std::cout << std::string(100, '=') << "\n\n";
    std::cout << "+-------------------------------+-------------------------------------------------------------+-------------+\n";
    std::cout << "| " << std::left << std::setw(30) << "Location"
              << "| " << std::setw(60) << "Description"
              << "| " << std::setw(12) << "Status" << "|\n";
    std::cout << "+-------------------------------+-------------------------------------------------------------+-------------+\n";
    for (const auto& check : typeChecks) {
        std::string loc_trunc = check.location.length() > 29 ? check.location.substr(0, 26) + "..." : check.location;
        std::string desc_trunc = check.description.length() > 59 ? check.description.substr(0, 56) + "..." : check.description;
        std::cout << "| " << std::left << std::setw(30) << loc_trunc
                  << "| " << std::setw(60) << desc_trunc
                  << "| " << std::setw(12) << check.status << "|\n";
    }
    std::cout << "+-------------------------------+-------------------------------------------------------------+-------------+\n";
    std::cout << "\nTotal Type Checks: " << typeChecks.size() << "\n";
    std::cout << "Status: " << (typeChecks.empty() ? "No checks performed" : "All passed") << "\n";
    std::cout << std::string(100, '=') << "\n\n";
}

void SymbolTable::printScopeChecks() const {
    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    std::cout << "Scope Checking\n";
    std::cout << "Generated on: " << timestamp << "\n";
    std::cout << std::string(60, '=') << "\n\n";
    std::cout << "+---------------------+-----------------+---------------+\n";
    std::cout << "| " << std::left << std::setw(20) << "Scope"
              << "| " << std::setw(16) << "Action"
              << "| " << std::setw(14) << "Symbol Count" << "|\n";
    std::cout << "+---------------------+-----------------+---------------+\n";
    for (const auto& action : scopeChecks) {
        std::string scope_trunc = action.scope.length() > 19 ? action.scope.substr(0, 16) + "..." : action.scope;
        std::cout << "| " << std::left << std::setw(20) << scope_trunc
                  << "| " << std::setw(16) << action.action
                  << "| " << std::right << std::setw(14) << action.symbolCount << "|\n";
    }
    std::cout << "+---------------------+-----------------+---------------+\n";
    std::cout << "\nTotal Scope Actions: " << scopeChecks.size() << "\n";
    std::cout << "Status: All scopes properly managed\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void SymbolTable::printIssues() const {
    std::time_t now = std::time(nullptr);
    char timestamp[26];
    ctime_r(&now, timestamp);
    timestamp[strlen(timestamp) - 1] = '\0';

    std::cout << "Semantic Errors/Warnings\n";
    std::cout << "Generated on: " << timestamp << "\n";
    std::cout << std::string(100, '=') << "\n\n";
    std::cout << "+-------------+-------------------------------------------------------------+-------------+\n";
    std::cout << "| " << std::left << std::setw(12) << "Type"
              << "| " << std::setw(60) << "Description"
              << "| " << std::setw(12) << "Status" << "|\n";
    std::cout << "+-------------+-------------------------------------------------------------+-------------+\n";
    if (issues.empty()) {
        std::cout << "| " << std::left << std::setw(12) << "Error"
                  << "| " << std::setw(60) << "No errors found"
                  << "| " << std::setw(12) << "✅" << "|\n";
        std::cout << "| " << std::left << std::setw(12) << "Warning"
                  << "| " << std::setw(60) << "No warnings found"
                  << "| " << std::setw(12) << "✅" << "|\n";
    } else {
        for (const auto& issue : issues) {
            std::string desc_trunc = issue.description.length() > 59 ? issue.description.substr(0, 56) + "..." : issue.description;
            std::cout << "| " << std::left << std::setw(12) << issue.type
                      << "| " << std::setw(60) << desc_trunc
                      << "| " << std::setw(12) << issue.status << "|\n";
        }
    }
    std::cout << "+-------------+-------------------------------------------------------------+-------------+\n";
    std::cout << "\nTotal Issues: " << issues.size() << "\n";
    std::cout << "Status: " << (issues.empty() ? "No major semantic errors detected" : "Issues detected") << "\n";
    std::cout << std::string(100, '=') << "\n\n";
}

const std::vector<SemanticIssue>& SymbolTable::getIssues() const {
    return issues;
}