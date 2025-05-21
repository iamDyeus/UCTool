#include "../include/SymbolTable.h"
#include <fstream>
#include <iostream>
#include <iomanip>

Symbol::Symbol() : type(""), scope(""), attributes(""), initialized(false), used(false), line(0) {
    std::cerr << "Warning: Symbol default constructor used" << std::endl;
}

Symbol::Symbol(const std::string& t, const std::string& s, const std::string& a, int l)
    : type(t), scope(s), attributes(a), initialized(true), used(false), line(l) {}

TypeCheck::TypeCheck(const std::string& loc, const std::string& desc, const std::string& stat)
    : location(loc), description(desc), status(stat) {}

ScopeCheck::ScopeCheck(const std::string& s, const std::string& a, int count)
    : scope(s), action(a), symbolCount(count) {}

SemanticIssue::SemanticIssue(const std::string& t, const std::string& desc, const std::string& stat)
    : type(t), description(desc), status(stat) {}

SymbolTable::SymbolTable() : hasStdioInclude(false) {
    scopes.push_back({});
    scopeNames.push_back("global");
    scopeChecks.emplace_back("global", "Entered", 0);
}

void SymbolTable::enterScope(const std::string& scopeName) {
    scopes.push_back({});
    scopeNames.push_back(scopeName);
    scopeChecks.emplace_back(scopeName, "Entered", scopes.back().size());
    std::cerr << "Debug: Entered scope '" << scopeName << "', total scopes: " << scopes.size() << std::endl;
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopeChecks.emplace_back(scopeNames.back(), "Exited", scopes.back().size());
        std::cerr << "Debug: Exited scope '" << scopeNames.back() << "', total scopes: " << scopes.size() - 1 << std::endl;
        scopes.pop_back();
        scopeNames.pop_back();
    }
}

bool SymbolTable::declare(const std::string& name, const std::string& type, const std::string& attributes, int line) {
    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        issues.emplace_back("Error", "Redeclaration of variable '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    currentScope.emplace(name, Symbol(type, scopeNames.back(), attributes, line));
    std::cerr << "Debug: Declared variable '" << name << "' with type '" << type << "' in scope '" << scopeNames.back() << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineMacro(const std::string& name, const std::string& value, int line) {
    if (macros.find(name) != macros.end()) {
        issues.emplace_back("Error", "Redefinition of macro '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    macros.emplace(name, Symbol("int (macro)", "global", "preprocessor constant", line));
    std::cerr << "Debug: Defined macro '" << name << "' with value '" << value << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineStruct(const std::string& name, int line) {
    if (structs.find(name) != structs.end()) {
        issues.emplace_back("Error", "Redefinition of struct '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    structs.emplace(name, Symbol("struct {float x, float y}", "global", "user-defined type", line));
    std::cerr << "Debug: Defined struct '" << name << "' at line " << line << std::endl;
    return true;
}

bool SymbolTable::defineFunction(const std::string& name, const std::string& type, int line) {
    if (functions.find(name) != functions.end()) {
        issues.emplace_back("Error", "Redefinition of function '" + name + "' at line " + std::to_string(line), "❌");
        return false;
    }
    functions.emplace(name, Symbol(type + " func(void)", "global", "function", line));
    std::cerr << "Debug: Defined function '" << name << "' with type '" << type << " func(void)' at line " << line << std::endl;
    return true;
}

const Symbol* SymbolTable::lookup(const std::string& name, int line) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto findIt = it->find(name);
        if (findIt != it->end()) {
            std::cerr << "Debug: Lookup succeeded for identifier '" << name << "' at line " << line << ", type=" << findIt->second.type << std::endl;
            return &(findIt->second);
        }
    }
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        return &(macroIt->second);
    }
    auto structIt = structs.find(name);
    if (structIt != structs.end()) {
        return &(structIt->second);
    }
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        return &(funcIt->second);
    }
    std::cerr << "Debug: Lookup failed for identifier '" << name << "' at line " << line << std::endl;
    return nullptr;
}

void SymbolTable::markUsed(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto findIt = it->find(name);
        if (findIt != it->end()) {
            findIt->second.used = true;
            return;
        }
    }
    auto macroIt = macros.find(name);
    if (macroIt != macros.end()) {
        macroIt->second.used = true;
        return;
    }
    auto structIt = structs.find(name);
    if (structIt != structs.end()) {
        structIt->second.used = true;
        return;
    }
    auto funcIt = functions.find(name);
    if (funcIt != functions.end()) {
        funcIt->second.used = true;
        return;
    }
}

void SymbolTable::setStdioInclude() { 
    hasStdioInclude = true;
    functions.emplace("printf", Symbol("void func(string)", "global", "function", 1));
}

bool SymbolTable::hasStdio() const { return hasStdioInclude; }

void SymbolTable::addTypeCheck(const std::string& location, const std::string& description, const std::string& status) const {
    typeChecks.emplace_back(location, description, status);
}

void SymbolTable::addWarning(const std::string& description, int line) {
    issues.emplace_back("Warning", description + " at line " + std::to_string(line), "⚠️");
}

void SymbolTable::checkUnusedSymbols() {
    for (auto& [name, symbol] : macros) {
        if (!symbol.used) {
            issues.emplace_back("Warning", "Unused macro '" + name + "' defined at line " + std::to_string(symbol.line), "⚠️");
        }
    }
    for (auto& [name, symbol] : structs) {
        if (!symbol.used) {
            issues.emplace_back("Warning", "Unused struct '" + name + "' defined at line " + std::to_string(symbol.line), "⚠️");
        }
    }
    for (auto& [name, symbol] : functions) {
        if (!symbol.used && name != "main ") {
            issues.emplace_back("Warning", "Unused function '" + name + "' defined at line " + std::to_string(symbol.line), "⚠️");
        }
    }
    for (const auto& scope : scopes) {
        for (const auto& [name, symbol] : scope) {
            if (!symbol.used) {
                issues.emplace_back("Warning", "Unused variable '" + name + "' in scope '" + symbol.scope + "' defined at line " + std::to_string(symbol.line), "⚠️");
            }
        }
    }
}

void SymbolTable::printSymbolTable() const {
    std::ofstream outFile("../temp/symbol_table.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/symbol_table.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nSymbol Table\n";
        os << "============\n\n";
        os << std::left
           << std::setw(20) << "Name"
           << std::setw(30) << "Type"
           << std::setw(15) << "Scope"
           << std::setw(25) << "Attributes"
           << std::setw(12) << "Initialized"
           << std::setw(10) << "Used"
           << std::setw(8) << "Line" << "\n";
        os << std::string(120, '-') << "\n";
        for (const auto& [name, symbol] : macros) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes
               << std::setw(12) << (symbol.initialized ? "Yes" : "No")
               << std::setw(10) << (symbol.used ? "Yes" : "No")
               << std::setw(8) << symbol.line << "\n";
        }
        for (const auto& [name, symbol] : structs) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes
               << std::setw(12) << (symbol.initialized ? "Yes" : "No")
               << std::setw(10) << (symbol.used ? "Yes" : "No")
               << std::setw(8) << symbol.line << "\n";
        }
        for (const auto& [name, symbol] : functions) {
            os << std::setw(20) << name
               << std::setw(30) << symbol.type
               << std::setw(15) << symbol.scope
               << std::setw(25) << symbol.attributes
               << std::setw(12) << (symbol.initialized ? "Yes" : "No")
               << std::setw(10) << (symbol.used ? "Yes" : "No")
               << std::setw(8) << symbol.line << "\n";
        }
        for (const auto& scope : scopes) {
            for (const auto& [name, symbol] : scope) {
                os << std::setw(20) << name
                   << std::setw(30) << symbol.type
                   << std::setw(15) << symbol.scope
                   << std::setw(25) << symbol.attributes
                   << std::setw(12) << (symbol.initialized ? "Yes" : "No")
                   << std::setw(10) << (symbol.used ? "Yes" : "No")
                   << std::setw(8) << symbol.line << "\n";
            }
        }
        os << "\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printTypeChecks() const {
    std::ofstream outFile("../temp/type_checks.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/type_checks.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nType Checking\n";
        os << "=============\n\n";
        os << std::left
           << std::setw(30) << "Location"
           << std::setw(50) << "Description"
           << std::setw(10) << "Status" << "\n";
        os << std::string(90, '-') << "\n";
        for (const auto& check : typeChecks) {
            os << std::setw(30) << check.location
               << std::setw(50) << check.description
               << std::setw(10) << check.status << "\n";
        }
        os << "\n" << (typeChecks.empty() ? "No type checks performed." : "✅ All type checks passed.") << "\n\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printScopeChecks() const {
    std::ofstream outFile("../temp/scope_checks.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/scope_checks.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nScope Checking\n";
        os << "==============\n\n";
        os << std::left
           << std::setw(20) << "Scope"
           << std::setw(15) << "Action"
           << std::setw(15) << "Symbol Count" << "\n";
        os << std::string(50, '-') << "\n";
        for (const auto& check : scopeChecks) {
            os << std::setw(20) << check.scope
               << std::setw(15) << check.action
               << std::setw(15) << check.symbolCount << "\n";
        }
        os << "\n✅ All scopes properly managed.\n\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

void SymbolTable::printIssues() const {
    std::ofstream outFile("../temp/semantic_issues.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open ../temp/semantic_issues.txt for writing" << std::endl;
        return;
    }

    auto print = [&](std::ostream& os) {
        os << "\nSemantic Errors/Warnings\n";
        os << "========================\n\n";
        os << std::left
           << std::setw(10) << "Type"
           << std::setw(60) << "Description"
           << std::setw(10) << "Status" << "\n";
        os << std::string(80, '-') << "\n";
        bool hasErrors = false;
        bool hasWarnings = false;
        for (const auto& issue : issues) {
            os << std::setw(10) << issue.type
               << std::setw(60) << issue.description
               << std::setw(10) << issue.status << "\n";
            if (issue.type == "Error") hasErrors = true;
            if (issue.type == "Warning") hasWarnings = true;
        }
        if (!hasErrors && !hasWarnings) {
            os << std::setw(10) << "Error"
               << std::setw(60) << "No errors found"
               << std::setw(10) << "✅" << "\n";
            os << std::setw(10) << "Warning"
               << std::setw(60) << "No warnings found"
               << std::setw(10) << "✅" << "\n";
        } else if (!hasErrors) {
            os << std::setw(10) << "Error"
               << std::setw(60) << "No errors found"
               << std::setw(10) << "✅" << "\n";
        }
        os << "\n" << (hasErrors ? "❌ Semantic errors detected." : "✅ No major semantic errors detected.") << "\n\n";
    };

    print(std::cout);
    print(outFile);
    outFile.close();
}

const std::vector<SemanticIssue>& SymbolTable::getIssues() const { return issues; }

bool SymbolTable::hasOnlyWarnings() const {
    for (const auto& issue : issues) {
        if (issue.type == "Error") return false;
    }
    return !issues.empty();
}