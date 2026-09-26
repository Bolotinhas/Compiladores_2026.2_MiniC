// token.hpp - tokens do MiniC++ e o tipo de erro usado por todas as fases.
#pragma once

#include <stdexcept>
#include <string>

namespace minicpp {

// Posicao no codigo-fonte (comeca em 1:1). Toda mensagem de erro carrega uma.
struct Pos {
    int line = 1;
    int col = 1;
};

enum class TokenKind {
    // literais e identificadores
    IntLit, DoubleLit, StringLit, Ident,
    // palavras-chave
    KwInt, KwDouble, KwBool, KwString, KwVoid, KwAuto, KwConst,
    KwIf, KwElse, KwWhile, KwFor, KwBreak, KwContinue, KwReturn,
    KwTrue, KwFalse,
    // operadores
    Plus, Minus, Star, Slash, Percent,
    Assign, PlusAssign, MinusAssign,
    EqEq, NotEq, Lt, Le, Gt, Ge,
    AndAnd, OrOr, Not,
    PlusPlus, MinusMinus,
    Shl,   // <<  (usado por cout << ...)
    Amp,   // &   (usado em tipos de referencia: int& x)
    // pontuacao
    LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon,
    Eof
};

struct Token {
    TokenKind kind = TokenKind::Eof;
    // Texto do token. Para StringLit, guarda o conteudo ja decodificado
    // (sem aspas, com \n, \t etc. ja convertidos).
    std::string lexeme;
    Pos pos;
};

// Nome legivel do token, para mensagens de erro e debug (ex.: "'+'", "identificador").
const char* to_string(TokenKind kind);

// Erro de compilacao (lexico ou sintatico), sempre com posicao.
class CompileError : public std::runtime_error {
public:
    CompileError(const std::string& message, Pos pos)
        : std::runtime_error(std::to_string(pos.line) + ":" + std::to_string(pos.col) +
                             ": " + message),
          pos_(pos),
          message_(message) {}

    Pos pos() const { return pos_; }
    const std::string& message() const { return message_; }

private:
    Pos pos_;
    std::string message_;
};

}  // namespace minicpp
