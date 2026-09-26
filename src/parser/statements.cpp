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

#include "parser.hpp"

namespace minicpp {

// (implementacao aqui)

}  // namespace minicpp
