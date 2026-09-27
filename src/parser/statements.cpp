// statements.cpp - parse de comandos: bloco, if/else, while, for, break,
// continue, return, e "comando de expressao" (uma expressao seguida de ';',
// como uma chamada de funcao ou uma atribuicao usadas como comando).
//
// TODO(grupo): implementar as funcoes abaixo. Sugestao de ordem: block,
// depois if/while (parecidos), depois for (o mais chato), depois o resto.
//
//   StmtPtr Parser::parse_statement() {
//       // olha o token atual (current().kind) e decide para qual das
//       // funcoes abaixo despachar. Se nao bater com nenhum comando
//       // conhecido, tenta "expr ';'" (comando de expressao).
//   }
//
//   StmtPtr Parser::parse_block() {
//       // consome '{', chama parse_statement() em loop ate achar '}'
//   }
//
//   StmtPtr Parser::parse_if() {
//       // consome 'if' '(' expr ')' statement ['else' statement]
//       // repare: o corpo NAO precisa ser obrigatoriamente um bloco em
//       // C++ (se ($x) y = 1; e valido), decidam se vao exigir bloco ou nao
//   }
//
//   StmtPtr Parser::parse_while() {
//       // consome 'while' '(' expr ')' statement
//   }
//
//   StmtPtr Parser::parse_for() {
//       // consome 'for' '(' [init] ';' [cond] ';' [step] ')' statement
//       // 'init' pode ser uma declaracao (int i = 0) OU uma expressao.
//       // dica: se o token atual for um dos tipos (KwInt, KwDouble, ...),
//       // e uma declaracao; senao, e uma expressao (ou vazio, se already ';')
//   }
//
//   StmtPtr Parser::parse_return() {
//       // consome 'return' [expr] ';'
//   }
//
// Depois de implementar cada metodo, descomente a declaracao correspondente
// em parser.hpp.

// feito: block, if, while

#include "parser.hpp"

namespace minicpp {

StmtPtr Parser::parse_statement() {
    Token t = current();

    switch (t.kind) {
        case TokenKind::LBrace:
            return parse_block();
        case TokenKind::KwIf:
            return parse_if();
        case TokenKind::KwWhile:
            return parse_while();
        case TokenKind::KwInt:
        case TokenKind::KwDouble:
        case TokenKind::KwBool:
        case TokenKind::KwString:
        case TokenKind::KwVoid:
        case TokenKind::KwAuto:
        case TokenKind::KwConst:
            return parse_var_decl();
        default: {
            // Comando de Expressao (ex: x = 5; ou f();)
            Pos pos = t.pos;
            ExprPtr expr = parse_expression();
            expect(TokenKind::Semicolon);
            return make_stmt(pos, ExprStmt{std::move(expr)});
        }
    }
}

// esqueceu de me block no parser
StmtPtr Parser::parse_block() {
    // Consome '{'
    Pos pos = expect(TokenKind::LBrace).pos;
    std::vector stmts;

    // Chama parse_statement() em loop ate achar '}' ou o fim do arquivo
    while (!check(TokenKind::RBrace) && !at_end()) {
        stmts.push_back(parse_statement());
    }

    // Consome '}'
    expect(TokenKind::RBrace);
    return make_stmt(pos, Block{std::move(stmts)});
}

// if 
StmtPtr Parser::parse_if() {
    // Consome 'if' '(' expr ')'
    Pos pos = expect(TokenKind::KwIf).pos;
    expect(TokenKind::LParen);
    ExprPtr cond = parse_expression();
    expect(TokenKind::RParen);

    // Parse do comando 'then' (pode ser um bloco ou um comando simples na linha)
    StmtPtr then_branch = parse_statement();
    StmtPtr else_branch = nullptr;

    // Trata o 'else' opcional
    if (check(TokenKind::KwElse)) {
        advance(); // consome 'else'
        else_branch = parse_statement();
    }

    return make_stmt(pos, If{std::move(cond), std::move(then_branch), std::move(else_branch)});
}

// while 
StmtPtr Parser::parse_while() {
    // Consome 'while' '(' expr ')'
    Pos pos = expect(TokenKind::KwWhile).pos;
    expect(TokenKind::LParen);
    ExprPtr cond = parse_expression();
    expect(TokenKind::RParen);

    // Parse do corpo do loop
    StmtPtr body = parse_statement();

    return make_stmt(pos, While{std::move(cond), std::move(body)});
}

}  // namespace minicpp
