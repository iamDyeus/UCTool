#ifndef LEXER_UTILS_HPP
#define LEXER_UTILS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "token_iterator.hpp" // For Tokens, UnknownTokens, TokenIterator

inline int col_num = 1;
extern int line_num;

inline std::vector<Tokens> tokens;
inline std::vector<UnknownTokens> unknown_tokens;
inline std::unordered_map<std::string, std::string> macros;
inline std::vector<std::string> included_files;

inline void define_macro(const std::string& name, const std::string& value) {
    macros[name] = value;
}

inline void include_file(const std::string& filename, bool is_system) {
    included_files.push_back(filename);
}

inline std::string expand_macro(const std::string& name) {
    auto it = macros.find(name);
    if (it != macros.end()) {
        return it->second;
    }
    return name;
}

#endif // LEXER_UTILS_HPP