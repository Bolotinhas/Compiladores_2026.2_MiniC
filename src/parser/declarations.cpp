// declarations.cpp - parse de declaracao de variavel:
//   ['const'] type IDENT ['=' expr] ';'
//
// Exemplos que devem ser aceitos:
//   int x = 5;
//   auto y = f(2);          (auto SEMPRE precisa de inicializador - essa
//                             regra pode ficar so para o type checker,
//                             entrega 3, nao precisa checar aqui no parser)
//   const int MAX = 10;
//   int contador;            (sem inicializador, se decidirem permitir)
//
// TODO(grupo): implementar
//
//   StmtPtr Parser::parse_var_decl() {
//       ...
//   }
//
// Dicas:
//   - Usa parse_type() (de types.cpp) para ler o tipo.
//   - Usa parse_expression() (ja pronto) para ler o valor apos o '='.
//   - O no da AST e VarDecl, em ast.hpp: { TypeRef type; string name; ExprPtr init; }
//   - Depois de implementar, descomente a declaracao em parser.hpp.

// declarations.cpp - parse de declaracao de variavel:
//   ['const'] type IDENT ['=' expr] ';'

#include "parser.hpp"

namespace minicpp {

StmtPtr Parser::parse_var_decl() {
    Pos pos = current().pos;

    // 1. Le o tipo usando o parse_type()
    TypeRef type = parse_type();

    // 2. Le o nome da variavel
    Token id = expect(TokenKind::Ident);

    // 3. Le a inicialização opcional '=' expr
    ExprPtr init = nullptr;
    if (check(TokenKind::Assign)) {
        advance();
        init = parse_expression();
    }

    // 4. Exige ';' no final
    expect(TokenKind::Semicolon);

    return make_stmt(pos, VarDecl{type, id.lexeme, std::move(init)});
}

}  // namespace minicpp
