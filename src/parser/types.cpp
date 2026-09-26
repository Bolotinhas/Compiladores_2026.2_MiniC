// types.cpp - parse de tipos: int, double, bool, string, void, auto,
//             'const' type, type '&' (referencia, so em parametros por
//             enquanto - ver ast.hpp / TypeRef).
//
// TODO(grupo): implementar
//
//   TypeRef Parser::parse_type() {
//       ...
//   }
//
// Dicas:
//   - Os tokens ja existem em token.hpp: KwInt, KwDouble, KwBool, KwString,
//     KwVoid, KwAuto, KwConst, Amp.
//   - 'const' vem ANTES do tipo:      const int x = 5;
//   - '&' vem DEPOIS do tipo:         void f(int& x)
//   - Erro de exemplo: "int&& x" (dois &) deveria dar CompileError.
//   - Depois de implementar, adicione a declaracao real do metodo em
//     parser.hpp (troque o comentario "// TypeRef parse_type();" por
//     "TypeRef parse_type();" sem o //).

#include "parser.hpp"

namespace minicpp {

// (implementacao aqui)

}  // namespace minicpp
