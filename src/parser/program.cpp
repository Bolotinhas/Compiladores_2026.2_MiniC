// program.cpp - parse do programa inteiro: uma lista de funcoes, uma atras
// da outra, ate acabar o arquivo.
//
// TODO(grupo): implementar
//
//   Program Parser::parse_program() {
//       Program prog;
//       while (!at_end()) {
//           prog.functions.push_back(parse_function());
//       }
//       return prog;
//   }
//
// IMPORTANTE (isso foi um bug real no MiniC original, em Rust): o loop
// acima PRECISA rodar ate current().kind == TokenKind::Eof. Se alguma
// funcao malformada fizer parse_function() parar no meio, sem consumir
// tudo, e o loop for embora sem checar isso, um erro de sintaxe pode
// silenciosamente sumir (o programa fica "incompleto" sem avisar ninguem).
// Aqui isso nao acontece porque parse_function() sempre lanca CompileError
// se nao conseguir formar uma funcao valida - so tomem cuidado ao
// implementar para manter essa garantia.
//
// Depois de implementar, descomente a declaracao em parser.hpp e
// implemente tambem:
//
//   Program parse_program_string(const std::string& source) {
//       Parser parser(Lexer(source).tokenize());
//       return parser.parse_program();
//       // (parse_program ja consome ate o Eof, entao nao precisa checar
//       // "sobrou texto" de novo aqui, diferente de parse_expression_string)
//   }

#include "parser.hpp"

namespace minicpp {

// (implementacao aqui)

}  // namespace minicpp
