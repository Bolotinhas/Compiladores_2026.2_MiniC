// functions.cpp - parse de uma funcao completa:
//   type IDENT '(' [param (',' param)*] ')' bloco
//   param := ['const'] type ['&'] IDENT
//
// Exemplo:
//   int fatorial(int n) { ... }
//   void incrementa(int& x) { ... }
//
// TODO(grupo): implementar
//
//   Param Parser::parse_param() {
//       // usa parse_type() (types.cpp) e depois le o IDENT do nome
//   }
//
//   Function Parser::parse_function() {
//       // usa parse_type() para o tipo de retorno, le o IDENT do nome,
//       // le a lista de parametros entre parenteses (separados por ','),
//       // e usa parse_block() (statements.cpp) para o corpo
//   }
//
// Depois de implementar, descomente as declaracoes em parser.hpp.

#include "parser.hpp"

namespace minicpp {

// (implementacao aqui)

}  // namespace minicpp
