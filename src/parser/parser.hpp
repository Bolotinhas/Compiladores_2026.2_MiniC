// parser.hpp - declaracao da classe Parser.
//
// A CLASSE e uma so (uma "caixa" com todos os metodos de parse), mas a
// IMPLEMENTACAO dos metodos fica espalhada em varios arquivos .cpp dentro
// desta pasta, um por assunto da gramatica:
//
//   expressions.cpp    -> parse_expression()                    [PRONTO]
//   types.cpp          -> parse_type()                          [TODO]
//   declarations.cpp   -> parse_var_decl()                      [TODO]
//   statements.cpp     -> parse_statement(), parse_block(), ...  [TODO]
//   functions.cpp      -> parse_function()                      [TODO]
//   program.cpp        -> parse_program()                       [TODO]
//
// Isso e so um detalhe de organizacao: em C++, os metodos de uma classe nao
// precisam estar todos no mesmo .cpp. O compilador junta tudo na hora de
// gerar o executavel. A vantagem e que cada pessoa do grupo mexe no seu
// arquivo sem mexer no arquivo dos outros.
//
// Quem for implementar um metodo TODO: so precisa escrever o corpo dele no
// .cpp correspondente (o metodo ja esta declarado aqui embaixo).
#pragma once

#include <string>
#include <vector>

#include "../ast.hpp"
#include "../token.hpp"

namespace minicpp {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // ------------------------------------------------------- expressions.cpp
    // Le UMA expressao a partir da posicao atual. [PRONTO]
    ExprPtr parse_expression();

    // ---------------------------------------------------------- types.cpp
    // TODO(grupo): le um tipo: int | double | bool | string | void | auto
    //              | 'const' type | type '&'
    TypeRef parse_type();

    // ----------------------------------------------------- declarations.cpp
    // TODO(grupo): le "['const'] type IDENT ['=' expr] ';'"
    StmtPtr parse_var_decl();

    // -------------------------------------------------------- statements.cpp
    // TODO(grupo): le qualquer comando (despacha para as funcoes abaixo
    //              olhando o token atual: '{' -> bloco, 'if' -> parse_if, etc.)
    // StmtPtr parse_statement();
    // StmtPtr parse_block();
    // StmtPtr parse_if();
    // StmtPtr parse_while();
    // StmtPtr parse_for();
    // StmtPtr parse_return();

    // --------------------------------------------------------- functions.cpp
    // TODO(grupo): le "type IDENT '(' [param (',' param)*] ')' bloco"
    // Function parse_function();
    // Param parse_param();

    // ----------------------------------------------------------- program.cpp
    // TODO(grupo): le function* ate o Eof (sem descartar texto sobrando,
    //              diferente do 'many0' do MiniC original)
    // Program parse_program();

    // --------------------------------------------------------------- estado
    // Usados pelos metodos de parse em qualquer arquivo desta pasta.
    const Token& current() const { return tokens_[pos_]; }
    bool at_end() const { return current().kind == TokenKind::Eof; }

private:
    // Helpers compartilhados (implementados em expressions.cpp por serem
    // usados primeiro ali, mas qualquer arquivo desta pasta pode chama-los).
    ExprPtr parse_expr_bp(int min_bp);  // "bp" = binding power (forca de ligacao)
    ExprPtr parse_prefix();
    ExprPtr parse_postfix(ExprPtr lhs);

    const Token& advance();
    bool check(TokenKind kind) const { return current().kind == kind; }
    const Token& expect(TokenKind kind);

    std::vector<Token> tokens_;
    std::size_t pos_ = 0;
};

// Conveniencia (usada nos testes e no main): le o texto, faz o parse de uma
// expressao e EXIGE que nao sobre nada depois dela.
ExprPtr parse_expression_string(const std::string& source);

// TODO(grupo), quando parse_program() existir:
// Program parse_program_string(const std::string& source);

}  // namespace minicpp
