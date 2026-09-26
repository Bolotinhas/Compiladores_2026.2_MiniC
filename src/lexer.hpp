// lexer.hpp - transforma texto em uma lista de tokens.
#pragma once

#include <string>
#include <vector>

#include "token.hpp"

namespace minicpp {

class Lexer {
public:
    explicit Lexer(std::string source) : src_(std::move(source)) {}

    // Le o codigo inteiro. A lista sempre termina com um token Eof.
    // Lanca CompileError em caso de erro lexico.
    std::vector<Token> tokenize();

private:
    Token next_token();
    void skip_trivia();  // espacos e comentarios (// e /* */)
    Token number(Pos start);
    Token word(Pos start);
    Token string_literal(Pos start);

    char peek(std::size_t offset = 0) const {
        return idx_ + offset < src_.size() ? src_[idx_ + offset] : '\0';
    }
    char advance();
    bool match(char expected);
    bool at_end() const { return idx_ >= src_.size(); }

    std::string src_;
    std::size_t idx_ = 0;
    int line_ = 1;
    int col_ = 1;
};

}  // namespace minicpp
