// test_parser_program.cpp - testes de parse_program() (src/parser/program.cpp).
//
// TODO(grupo): completar apos implementar. O caso mais importante e o de
// erro: garantir que um erro de sintaxe NUNCA seja silenciosamente
// descartado (era um bug real no MiniC original - ver README.md).
//
//   TEST(parser_programa_com_duas_funcoes) {
//       // parse_program_string("int f(){return 1;} int main(){return 0;}")
//       // deve devolver um Program com 2 funcoes
//   }
//
//   TEST(parser_programa_vazio_e_aceito) {
//       // parse_program_string("") deve devolver um Program sem funcoes
//       // (a checagem de que 'main' existe fica para o type checker,
//       // entrega 3 - nao e responsabilidade do parser)
//   }
//
//   TEST(parser_erro_de_sintaxe_no_meio_nao_pode_sumir) {
//       // um arquivo com uma funcao valida seguida de uma QUEBRADA deve
//       // lancar CompileError, e nao devolver so a primeira funcao calada
//       CHECK_ERROR(minicpp::parse_program_string(
//           "int f() { return 1; } int g( { return 2; }"), /*linha*/1, /*col*/29);
//   }

#include "mini_test.hpp"
#include "parser/parser.hpp"

// (testes aqui)
