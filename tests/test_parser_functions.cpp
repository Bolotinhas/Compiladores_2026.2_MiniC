// test_parser_functions.cpp - testes de parse_function() e parse_param()
// (src/parser/functions.cpp).
//
// TODO(grupo): completar apos implementar. Exemplos do que testar:
//
//   TEST(parser_funcao_sem_parametros) {
//       // "int f() { return 1; }" deve ser aceito
//   }
//
//   TEST(parser_funcao_com_parametros) {
//       // "int soma(int a, int b) { return a + b; }"
//   }
//
//   TEST(parser_parametro_referencia) {
//       // "void inc(int& x) { x++; }"
//   }
//
//   TEST(parser_parametro_const) {
//       // "void f(const int x) { }"
//   }
//
//   TEST(parser_erro_funcao_sem_tipo_de_retorno) {
//       // "f() { }" deve dar erro (falta o tipo antes do nome)
//   }

#include "mini_test.hpp"
#include "parser/parser.hpp"

// (testes aqui)
