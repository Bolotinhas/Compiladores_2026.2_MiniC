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

TypeRef Parser::parse_type() {
    TypeRef type;

    // 1. Opcional 'const' antes do tipo
    if (check(TokenKind::KwConst)) {
        advance();
        type.is_const = true;
    }

    // 2. Tipo base obrigatório
    Token t = current();
    switch (t.kind) {
        case TokenKind::KwInt:    type.base = BaseType::Int; break;
        case TokenKind::KwDouble: type.base = BaseType::Double; break;
        case TokenKind::KwBool:   type.base = BaseType::Bool; break;
        case TokenKind::KwString: type.base = BaseType::String; break;
        case TokenKind::KwVoid:   type.base = BaseType::Void; break;
        case TokenKind::KwAuto:   type.base = BaseType::Auto; break;
        default:
            throw CompileError("tipo esperado mas encontrado '" + t.lexeme + "'", t.pos);
    }
    advance();

    // 3. Opcional '&' depois do tipo
    if (check(TokenKind::Amp)) {
        advance();
        type.is_ref = true;
        
        // Trata erro de referencia dupla (ex: int&&)
        if (check(TokenKind::Amp)) {
            throw CompileError("referencias duplas ('&&') nao sao permitidas", current().pos);
        }
    }

    return type;
}

}  // namespace minicpp
