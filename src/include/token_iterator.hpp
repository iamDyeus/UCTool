#ifndef TOKEN_ITERATOR_HPP
#define TOKEN_ITERATOR_HPP

#include <string>
#include <vector>

struct Tokens {
    std::string type;
    std::string value;
    int line_no;
    int col_no;
};

struct UnknownTokens {
    std::string value;
    int line_no;
    int col_no;
};

class TokenIterator {
private:
    std::vector<Tokens> tokens;
    std::vector<UnknownTokens> unknown_tokens;
    size_t token_index;
    size_t unknown_token_index;
    Tokens temp_token; // For converting UnknownTokens to Tokens

public:
    TokenIterator(const std::vector<Tokens>& tokens, const std::vector<UnknownTokens>& unknown_tokens)
        : tokens(tokens), unknown_tokens(unknown_tokens), token_index(0), unknown_token_index(0) {}

    bool is_unknown_token() const {
        return unknown_token_index < unknown_tokens.size() && token_index >= tokens.size();
    }

    bool has_next() const {
        return token_index < tokens.size() || unknown_token_index < unknown_tokens.size();
    }

    Tokens* next() {
        if (is_unknown_token()) {
            const auto& ut = unknown_tokens[unknown_token_index++];
            temp_token.type = "Unknown";
            temp_token.value = ut.value;
            temp_token.line_no = ut.line_no;
            temp_token.col_no = ut.col_no;
            return &temp_token;
        }
        if (token_index < tokens.size()) {
            return &tokens[token_index++];
        }
        return nullptr;
    }
};

#endif // TOKEN_ITERATOR_HPP