// ast.hpp - arvore sintatica abstrata (AST) do MiniC++.
//
// Cada no de expressao/comando e um std::variant (equivalente aos enums do Rust)
// e os filhos sao std::unique_ptr (equivalente ao Box<T>).
// Para percorrer a arvore use std::visit (veja ast_printer.cpp).
#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "token.hpp"

namespace minicpp {

// ---------------------------------------------------------------- Expressoes

struct Expr;
using ExprPtr = std::unique_ptr<Expr>;

enum class UnaryOp { Neg, Not, PreInc, PreDec, PostInc, PostDec };
enum class BinaryOp { Add, Sub, Mul, Div, Mod, Eq, Ne, Lt, Le, Gt, Ge, And, Or, Shl };
enum class AssignOp { Assign, AddAssign, SubAssign };

struct IntLit    { long long value = 0; };
struct DoubleLit { double value = 0.0; };
struct StringLit { std::string value; };
struct BoolLit   { bool value = false; };
// 'cout' e 'endl' tambem sao Ident: quem trata os nomes especiais e o type checker.
struct Ident     { std::string name; };
struct Unary     { UnaryOp op; ExprPtr operand; };
struct Binary    { BinaryOp op; ExprPtr lhs; ExprPtr rhs; };
struct Assign    { AssignOp op; ExprPtr target; ExprPtr value; };
struct Call      { std::string callee; std::vector<ExprPtr> args; };
struct Index     { ExprPtr base; ExprPtr index; };

struct Expr {
    Pos pos;  // onde a expressao comeca (usado nas mensagens de erro)
    std::variant<IntLit, DoubleLit, StringLit, BoolLit, Ident,
                 Unary, Binary, Assign, Call, Index>
        node;
};

template <typename T>
ExprPtr make_expr(Pos pos, T node) {
    auto e = std::make_unique<Expr>();
    e->pos = pos;
    e->node = std::move(node);
    return e;
}

// ------------------------------------------------------------------- Comandos
// TODO(grupo): o parser ainda nao produz estes nos. Ajustem o formato antes de
// comecar a implementar, porque o type checker e o interpretador dependem dele.
//( acho q ajustei )

enum class BaseType { Int, Double, Bool, String, Void, Auto };

struct TypeRef {
    BaseType base = BaseType::Int;
    bool is_const = false;  // const T
    bool is_ref = false;    // T&  (so em parametros)
};

struct Stmt;
using StmtPtr = std::unique_ptr<Stmt>;

struct VarDecl  { TypeRef type; std::string name; ExprPtr init; };  // init pode ser nulo
struct ExprStmt { ExprPtr expr; };
struct Block    { std::vector<StmtPtr> stmts; };
struct If       { ExprPtr cond; StmtPtr then_branch; StmtPtr else_branch; };  // else pode ser nulo
struct While    { ExprPtr cond; StmtPtr body; };
struct For      { StmtPtr init; ExprPtr cond; ExprPtr step; StmtPtr body; };  // init/cond/step podem ser nulos
struct Break    {};
struct Continue {};
struct Return   { ExprPtr value; };  // value nulo em 'return;'

struct Stmt {
    Pos pos;
    std::variant<VarDecl, ExprStmt, Block, If, While, For, Break, Continue, Return> node;
};

// para criar nos de comando
template 
StmtPtr make_stmt(Pos pos, T node) {
    auto s = std::make_unique();
    s->pos = pos;
    s->node = std::move(node);
    return s;
}

// ------------------------------------------------------------------- Programa

struct Param {
    TypeRef type;
    std::string name;
};

struct Function {
    Pos pos;
    TypeRef return_type;
    std::string name;
    std::vector<Param> params;
    StmtPtr body;  // sempre um Block
};

struct Program {
    std::vector<Function> functions;
};

}  // namespace minicpp
