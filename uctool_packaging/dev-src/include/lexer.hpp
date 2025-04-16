#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// Stores macro definitions
extern unordered_map<string, string> macros;

// Tracks already included files
extern unordered_set<string> included_files;

// Add or update a macro definition
inline void define_macro(const string &name, const string &value) {
    macros[name] = value;
}

// Expand macro if defined
inline string expand_macro(const string &name) {
    auto it = macros.find(name);
    return (it != macros.end()) ? it->second : name;
}

// Include a file (simulate #include)
inline void include_file(const string &filename, bool is_system = false) {
    if (included_files.count(filename)) return;
    included_files.insert(filename);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open include file: " << filename << endl;
        return;
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();

    yypush_buffer_state(yy_scan_string(content.c_str()));
}

#endif
