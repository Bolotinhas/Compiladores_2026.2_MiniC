// expressions.cpp - parse de expressoes (Pratt parsing).
// Cobre: literais, identificadores, +-*/ %, comparacoes, && || !,
// chamadas f(...), indexacao a[i], ++ -- (pre e pos), = += -=, cout << ...
//
// Precedencia: tabela unica em infix_bp() logo abaixo. Para adicionar um
// operador novo, e so acrescentar uma linha nessa tabela (e, se for um
// prefixo como '-' e '!', um caso em parse_prefix()).
#include "parser.hpp"

#include "../lexer.hpp"

#include <optional>
#include <utility>


namespace minicpp {

namespace {

// ---------------------------------------------------------------------------
// Tabela de precedencia. Maior numero = liga mais forte.
//
// Cada operador infixo tem duas forcas: {esquerda, direita}.
//   esquerda < direita  -> associativo a esquerda   (a - b - c = (a - b) - c)
//   esquerda > direita  -> associativo a direita    (a = b = c = a = (b = c))
//
// Ordem, da mais fraca para a mais forte (igual ao C++):
//   = += -=  <  ||  <  &&  <  == !=  <  < <= > >=  <  <<  <  + -  <  * / %
//   depois os prefixos (- ! ++ --) e por fim os pos-fixos ( ) [ ] ++ --
// ---------------------------------------------------------------------------
constexpr int kPrefixBp = 17;
constexpr int kPostfixBp = 19;

struct InfixBp {
    int left;
    int right;
};

std::optional<InfixBp> infix_bp(TokenKind kind) {
    switch (kind) {
        case TokenKind::Assign:
        case TokenKind::PlusAssign:
        case TokenKind::MinusAssign: return InfixBp{2, 1};
        case TokenKind::OrOr: return InfixBp{3, 4};
        case TokenKind::AndAnd: return InfixBp{5, 6};
        case TokenKind::EqEq:
        case TokenKind::NotEq: return InfixBp{7, 8};
        case TokenKind::Lt:
        case TokenKind::Le:
        case TokenKind::Gt:
        case TokenKind::Ge: return InfixBp{9, 10};
        case TokenKind::Shl: return InfixBp{11, 12};
        case TokenKind::Plus:
        case TokenKind::Minus: return InfixBp{13, 14};
        case TokenKind::Star:
        case TokenKind::Slash:
        case TokenKind::Percent: return InfixBp{15, 16};
        default: return std::nullopt;
    }
}

bool is_postfix(TokenKind kind) {
    return kind == TokenKind::LParen || kind == TokenKind::LBracket ||
           kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
}

bool is_lvalue(const Expr& e) {
    return std::holds_alternative<Ident>(e.node) || std::holds_alternative<Index>(e.node);
}

std::string describe(const Token& t) {
    if (t.kind == TokenKind::Eof) return "fim do arquivo";
    return "'" + t.lexeme + "'";
}

ExprPtr build_binary(const Token& op, ExprPtr lhs, ExprPtr rhs) {
    BinaryOp bop;
    switch (op.kind) {
        case TokenKind::Plus: bop = BinaryOp::Add; break;
        case TokenKind::Minus: bop = BinaryOp::Sub; break;
        case TokenKind::Star: bop = BinaryOp::Mul; break;
        case TokenKind::Slash: bop = BinaryOp::Div; break;
        case TokenKind::Percent: bop = BinaryOp::Mod; break;
        case TokenKind::EqEq: bop = BinaryOp::Eq; break;
        case TokenKind::NotEq: bop = BinaryOp::Ne; break;
        case TokenKind::Lt: bop = BinaryOp::Lt; break;
        case TokenKind::Le: bop = BinaryOp::Le; break;
        case TokenKind::Gt: bop = BinaryOp::Gt; break;
        case TokenKind::Ge: bop = BinaryOp::Ge; break;
        case TokenKind::AndAnd: bop = BinaryOp::And; break;
        case TokenKind::OrOr: bop = BinaryOp::Or; break;
        case TokenKind::Shl: bop = BinaryOp::Shl; break;
        default: throw CompileError("operador binario desconhecido", op.pos);
    }
    Pos pos = lhs->pos;
    return make_expr(pos, Binary{bop, std::move(lhs), std::move(rhs)});
}

ExprPtr build_assign(const Token& op, ExprPtr target, ExprPtr value) {
    if (!is_lvalue(*target)) {
        throw CompileError("o lado esquerdo de " + std::string(to_string(op.kind)) +
                               " precisa ser uma variavel ou um elemento de array",
                           op.pos);
    }
    AssignOp aop = op.kind == TokenKind::Assign       ? AssignOp::Assign
                   : op.kind == TokenKind::PlusAssign ? AssignOp::AddAssign
                                                      : AssignOp::SubAssign;
    Pos pos = target->pos;
    return make_expr(pos, Assign{aop, std::move(target), std::move(value)});
}

}  // namespace

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
    // Garante o sentinela Eof no final, para current() nunca sair do vetor.
    if (tokens_.empty() || tokens_.back().kind != TokenKind::Eof) {
        Pos p = tokens_.empty() ? Pos{} : tokens_.back().pos;
        tokens_.push_back(Token{TokenKind::Eof, "", p});
    }
}

const Token& Parser::advance() {
    const Token& t = tokens_[pos_];
    if (t.kind != TokenKind::Eof) ++pos_;
    return t;
}

const Token& Parser::expect(TokenKind kind) {
    if (!check(kind)) {
        throw CompileError(std::string("esperado ") + to_string(kind) + " mas encontrado " +
                               describe(current()),
                           current().pos);
    }
    return advance();
}

ExprPtr Parser::parse_expression() { return parse_expr_bp(0); }

// Coracao do Pratt parser: le um operando e, enquanto o proximo operador
// ligar com forca >= min_bp, o consome e continua.
ExprPtr Parser::parse_expr_bp(int min_bp) {
    ExprPtr lhs = parse_prefix();
    for (;;) {
        TokenKind k = current().kind;

        if (is_postfix(k)) {
            if (kPostfixBp < min_bp) break;
            lhs = parse_postfix(std::move(lhs));
            continue;
        }

        auto bp = infix_bp(k);
        if (!bp || bp->left < min_bp) break;

        Token op = advance();
        ExprPtr rhs = parse_expr_bp(bp->right);
        if (op.kind == TokenKind::Assign || op.kind == TokenKind::PlusAssign ||
            op.kind == TokenKind::MinusAssign) {
            lhs = build_assign(op, std::move(lhs), std::move(rhs));
        } else {
            lhs = build_binary(op, std::move(lhs), std::move(rhs));
        }
    }
    return lhs;
}

ExprPtr Parser::parse_prefix() {
    Token t = advance();
    switch (t.kind) {
        case TokenKind::IntLit: {
            try {
                return make_expr(t.pos, IntLit{std::stoll(t.lexeme)});
            } catch (const std::out_of_range&) {
                throw CompileError("literal inteiro fora do intervalo", t.pos);
            }
        }
        case TokenKind::DoubleLit: {
            try {
                return make_expr(t.pos, DoubleLit{std::stod(t.lexeme)});
            } catch (const std::out_of_range&) {
                throw CompileError("literal double fora do intervalo", t.pos);
            }
        }
        case TokenKind::StringLit: return make_expr(t.pos, StringLit{t.lexeme});
        case TokenKind::KwTrue: return make_expr(t.pos, BoolLit{true});
        case TokenKind::KwFalse: return make_expr(t.pos, BoolLit{false});
        case TokenKind::Ident: return make_expr(t.pos, Ident{t.lexeme});

        case TokenKind::LParen: {
            ExprPtr inner = parse_expr_bp(0);
            expect(TokenKind::RParen);
            return inner;
        }

        case TokenKind::Minus:
            return make_expr(t.pos, Unary{UnaryOp::Neg, parse_expr_bp(kPrefixBp)});
        case TokenKind::Not:
            return make_expr(t.pos, Unary{UnaryOp::Not, parse_expr_bp(kPrefixBp)});
        case TokenKind::PlusPlus:
        case TokenKind::MinusMinus: {
            ExprPtr operand = parse_expr_bp(kPrefixBp);
            if (!is_lvalue(*operand)) {
                throw CompileError(std::string("o operando de ") + to_string(t.kind) +
                                       " precisa ser uma variavel ou um elemento de array",
                                   t.pos);
            }
            UnaryOp op = t.kind == TokenKind::PlusPlus ? UnaryOp::PreInc : UnaryOp::PreDec;
            return make_expr(t.pos, Unary{op, std::move(operand)});
        }

        default:
            throw CompileError("expressao esperada mas encontrado " + describe(t), t.pos);
    }
}

ExprPtr Parser::parse_postfix(ExprPtr lhs) {
    Token t = advance();
    Pos pos = lhs->pos;
    switch (t.kind) {
        case TokenKind::LParen: {
            auto* id = std::get_if<Ident>(&lhs->node);
            if (!id) throw CompileError("so e possivel chamar funcoes pelo nome", t.pos);
            std::vector<ExprPtr> args;
            if (!check(TokenKind::RParen)) {
                args.push_back(parse_expr_bp(0));
                while (check(TokenKind::Comma)) {
                    advance();
                    args.push_back(parse_expr_bp(0));
                }
            }
            expect(TokenKind::RParen);
            return make_expr(pos, Call{id->name, std::move(args)});
        }
        case TokenKind::LBracket: {
            ExprPtr index = parse_expr_bp(0);
            expect(TokenKind::RBracket);
            return make_expr(pos, Index{std::move(lhs), std::move(index)});
        }
        case TokenKind::PlusPlus:
        case TokenKind::MinusMinus: {
            if (!is_lvalue(*lhs)) {
                throw CompileError(std::string("o operando de ") + to_string(t.kind) +
                                       " precisa ser uma variavel ou um elemento de array",
                                   t.pos);
            }
            UnaryOp op = t.kind == TokenKind::PlusPlus ? UnaryOp::PostInc : UnaryOp::PostDec;
            return make_expr(pos, Unary{op, std::move(lhs)});
        }
        default:
            throw CompileError("operador pos-fixo inesperado", t.pos);
    }
}

ExprPtr parse_expression_string(const std::string& source) {
    Parser parser(Lexer(source).tokenize());
    ExprPtr expr = parser.parse_expression();
    if (!parser.at_end()) {
        throw CompileError("token inesperado " + describe(parser.current()),
                           parser.current().pos);
    }
    return expr;
}

}  // namespace minicpp
