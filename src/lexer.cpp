#include "lexer.hpp"

#include <cctype>
#include <unordered_map>

namespace minicpp {

const char* to_string(TokenKind kind) {
    switch (kind) {
        case TokenKind::IntLit: return "literal inteiro";
        case TokenKind::DoubleLit: return "literal double";
        case TokenKind::StringLit: return "literal string";
        case TokenKind::Ident: return "identificador";
        case TokenKind::KwInt: return "'int'";
        case TokenKind::KwDouble: return "'double'";
        case TokenKind::KwBool: return "'bool'";
        case TokenKind::KwString: return "'string'";
        case TokenKind::KwVoid: return "'void'";
        case TokenKind::KwAuto: return "'auto'";
        case TokenKind::KwConst: return "'const'";
        case TokenKind::KwIf: return "'if'";
        case TokenKind::KwElse: return "'else'";
        case TokenKind::KwWhile: return "'while'";
        case TokenKind::KwFor: return "'for'";
        case TokenKind::KwBreak: return "'break'";
        case TokenKind::KwContinue: return "'continue'";
        case TokenKind::KwReturn: return "'return'";
        case TokenKind::KwTrue: return "'true'";
        case TokenKind::KwFalse: return "'false'";
        case TokenKind::Plus: return "'+'";
        case TokenKind::Minus: return "'-'";
        case TokenKind::Star: return "'*'";
        case TokenKind::Slash: return "'/'";
        case TokenKind::Percent: return "'%'";
        case TokenKind::Assign: return "'='";
        case TokenKind::PlusAssign: return "'+='";
        case TokenKind::MinusAssign: return "'-='";
        case TokenKind::EqEq: return "'=='";
        case TokenKind::NotEq: return "'!='";
        case TokenKind::Lt: return "'<'";
        case TokenKind::Le: return "'<='";
        case TokenKind::Gt: return "'>'";
        case TokenKind::Ge: return "'>='";
        case TokenKind::AndAnd: return "'&&'";
        case TokenKind::OrOr: return "'||'";
        case TokenKind::Not: return "'!'";
        case TokenKind::PlusPlus: return "'++'";
        case TokenKind::MinusMinus: return "'--'";
        case TokenKind::Shl: return "'<<'";
        case TokenKind::Amp: return "'&'";
        case TokenKind::LParen: return "'('";
        case TokenKind::RParen: return "')'";
        case TokenKind::LBrace: return "'{'";
        case TokenKind::RBrace: return "'}'";
        case TokenKind::LBracket: return "'['";
        case TokenKind::RBracket: return "']'";
        case TokenKind::Comma: return "','";
        case TokenKind::Semicolon: return "';'";
        case TokenKind::Eof: return "fim do arquivo";
    }
    return "?";
}

namespace {

bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; }
bool is_alpha(char c) { return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_'; }
bool is_alnum(char c) { return is_alpha(c) || is_digit(c); }

Token make(TokenKind kind, std::string lexeme, Pos pos) {
    return Token{kind, std::move(lexeme), pos};
}

const std::unordered_map<std::string, TokenKind>& keywords() {
    static const std::unordered_map<std::string, TokenKind> table = {
        {"int", TokenKind::KwInt},           {"double", TokenKind::KwDouble},
        {"bool", TokenKind::KwBool},         {"string", TokenKind::KwString},
        {"void", TokenKind::KwVoid},         {"auto", TokenKind::KwAuto},
        {"const", TokenKind::KwConst},       {"if", TokenKind::KwIf},
        {"else", TokenKind::KwElse},         {"while", TokenKind::KwWhile},
        {"for", TokenKind::KwFor},           {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue}, {"return", TokenKind::KwReturn},
        {"true", TokenKind::KwTrue},         {"false", TokenKind::KwFalse},
    };
    return table;
}

}  // namespace

char Lexer::advance() {
    char c = src_[idx_++];
    if (c == '\n') {
        ++line_;
        col_ = 1;
    } else {
        ++col_;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (at_end() || src_[idx_] != expected) return false;
    advance();
    return true;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    for (;;) {
        Token t = next_token();
        bool done = t.kind == TokenKind::Eof;
        tokens.push_back(std::move(t));
        if (done) break;
    }
    return tokens;
}

void Lexer::skip_trivia() {
    for (;;) {
        char c = peek();
        if (!at_end() && std::isspace(static_cast<unsigned char>(c))) {
            advance();
        } else if (c == '/' && peek(1) == '/') {
            while (!at_end() && peek() != '\n') advance();
        } else if (c == '/' && peek(1) == '*') {
            Pos start{line_, col_};
            advance();
            advance();
            while (!(peek() == '*' && peek(1) == '/')) {
                if (at_end()) throw CompileError("comentario de bloco nao terminado", start);
                advance();
            }
            advance();
            advance();
        } else {
            return;
        }
    }
}

Token Lexer::number(Pos start) {
    std::size_t begin = idx_;
    while (is_digit(peek())) advance();
    bool is_double = false;
    if (peek() == '.' && is_digit(peek(1))) {
        is_double = true;
        advance();
        while (is_digit(peek())) advance();
    }
    if (is_alpha(peek())) throw CompileError("numero mal formado", start);
    return make(is_double ? TokenKind::DoubleLit : TokenKind::IntLit,
                src_.substr(begin, idx_ - begin), start);
}

Token Lexer::word(Pos start) {
    std::size_t begin = idx_;
    while (is_alnum(peek())) advance();
    std::string text = src_.substr(begin, idx_ - begin);
    auto it = keywords().find(text);
    TokenKind kind = it != keywords().end() ? it->second : TokenKind::Ident;
    return make(kind, std::move(text), start);
}

Token Lexer::string_literal(Pos start) {
    advance();  // aspas de abertura
    std::string value;
    for (;;) {
        if (at_end() || peek() == '\n') throw CompileError("string nao terminada", start);
        char c = advance();
        if (c == '"') break;
        if (c != '\\') {
            value += c;
            continue;
        }
        Pos esc{line_, col_ - 1};
        if (at_end()) throw CompileError("string nao terminada", start);
        char e = advance();
        switch (e) {
            case 'n': value += '\n'; break;
            case 't': value += '\t'; break;
            case 'r': value += '\r'; break;
            case '\\': value += '\\'; break;
            case '"': value += '"'; break;
            default:
                throw CompileError(std::string("sequencia de escape invalida '\\") + e + "'", esc);
        }
    }
    return make(TokenKind::StringLit, std::move(value), start);
}

Token Lexer::next_token() {
    skip_trivia();
    Pos start{line_, col_};
    if (at_end()) return make(TokenKind::Eof, "", start);

    char c = peek();
    if (is_digit(c)) return number(start);
    if (is_alpha(c)) return word(start);
    if (c == '"') return string_literal(start);

    advance();
    switch (c) {
        case '+':
            if (match('+')) return make(TokenKind::PlusPlus, "++", start);
            if (match('=')) return make(TokenKind::PlusAssign, "+=", start);
            return make(TokenKind::Plus, "+", start);
        case '-':
            if (match('-')) return make(TokenKind::MinusMinus, "--", start);
            if (match('=')) return make(TokenKind::MinusAssign, "-=", start);
            return make(TokenKind::Minus, "-", start);
        case '*': return make(TokenKind::Star, "*", start);
        case '/': return make(TokenKind::Slash, "/", start);
        case '%': return make(TokenKind::Percent, "%", start);
        case '=':
            if (match('=')) return make(TokenKind::EqEq, "==", start);
            return make(TokenKind::Assign, "=", start);
        case '!':
            if (match('=')) return make(TokenKind::NotEq, "!=", start);
            return make(TokenKind::Not, "!", start);
        case '<':
            if (match('=')) return make(TokenKind::Le, "<=", start);
            if (match('<')) return make(TokenKind::Shl, "<<", start);
            return make(TokenKind::Lt, "<", start);
        case '>':
            if (match('=')) return make(TokenKind::Ge, ">=", start);
            return make(TokenKind::Gt, ">", start);
        case '&':
            if (match('&')) return make(TokenKind::AndAnd, "&&", start);
            return make(TokenKind::Amp, "&", start);
        case '|':
            if (match('|')) return make(TokenKind::OrOr, "||", start);
            throw CompileError("caractere inesperado '|' (voce quis dizer '||'?)", start);
        case '(': return make(TokenKind::LParen, "(", start);
        case ')': return make(TokenKind::RParen, ")", start);
        case '{': return make(TokenKind::LBrace, "{", start);
        case '}': return make(TokenKind::RBrace, "}", start);
        case '[': return make(TokenKind::LBracket, "[", start);
        case ']': return make(TokenKind::RBracket, "]", start);
        case ',': return make(TokenKind::Comma, ",", start);
        case ';': return make(TokenKind::Semicolon, ";", start);
        default: break;
    }
    throw CompileError(std::string("caractere inesperado '") + c + "'", start);
}

}  // namespace minicpp
