// test_parser_statements.cpp - testes de if, while, for, break, continue,
// return, bloco e declaracao de variavel.
//
// TODO(grupo): descomentar e completar conforme parse_statement() (em
// src/parser/statements.cpp) e parse_var_decl() (em
// src/parser/declarations.cpp) forem sendo implementados.
//
// Um jeito pratico de testar comandos e imprimir a AST tambem para Stmt
// (adicionando um to_sexpr(const Stmt&) em ast_printer.*, parecido com o
// que ja existe para Expr) e comparar a string resultante, igual e feito
// em test_parser_expressions.cpp.
//
// Exemplos do que testar aqui:
//
//   TEST(parser_if_simples) {
//       CHECK_EQ(sx_stmt("if (x < 3) { y = 1; }"), "(if (< x 3) (block (= y 1)))");
//   }
//
//   TEST(parser_if_else) {
//       CHECK_EQ(sx_stmt("if (x) { a; } else { b; }"), "(if x (block a) (block b))");
//   }
//
//   TEST(parser_while) {
//       CHECK_EQ(sx_stmt("while (x < 10) { x++; }"), "(while (< x 10) (block (post++ x)))");
//   }
//
//   TEST(parser_for) {
//       CHECK_EQ(sx_stmt("for (int i = 0; i < 10; i++) { }"), "...");
//   }
//
//   TEST(parser_erro_if_sem_parenteses) {
//       CHECK_ERROR(sx_stmt("if x < 3 { y = 1; }"), 1, 4);
//   }

#include "mini_test.hpp"
#include "parser/parser.hpp"

// (testes aqui)
