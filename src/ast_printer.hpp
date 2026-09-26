// ast_printer.hpp - imprime a AST como S-expression, ex.: (+ 1 (* 2 3)).
// Usado pelos testes (comparar a estrutura da arvore) e pela flag --expr.
#pragma once

#include <string>

#include "ast.hpp"

namespace minicpp {

std::string to_sexpr(const Expr& expr);

}  // namespace minicpp
