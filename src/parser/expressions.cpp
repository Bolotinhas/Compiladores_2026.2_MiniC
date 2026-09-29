// expressions.cpp - parse de expressoes (Pratt parsing).
// Cobre: literais, identificadores, + - * / %, comparacoes, && || !,
// chamadas f(...), indexacao a[i], ++ -- (pre e pos), = += -=, cout << ...,
// e o operador ternario (cond ? exp1 : exp2).

#include "parser.hpp"
#include 
#include 
#include 

namespace minicpp {

namespace {

// ---------------------------------------------------------------------------
// Tabela de precedencia
// Cada operador infixo tem duas forcas: {esquerda, direita}.
//   esquerda < direita  -> associativo a esquerda   (a - b - c = (a - b) - c)
//   esquerda > direita  -> associativo a direita    (a = b = c = a = (b = c))
// ---------------------------------------------------------------------------
constexpr int kPrefixBp = 17;
constexpr int kPostfixBp = 19;

struct InfixBp {
    int left;
    int right;
};

std::optional infix_bp(TipoToken tipo) {
    switch (tipo) {
        // Atribuicao (associativa a direita)
        case TipoToken::Assign:
        case TipoToken::PlusAssign:
        case TipoToken::MinAssign: return InfixBp{2, 1};

        // Ternario ? : (associativo a direita)
        case TipoToken::Interrogacao: return InfixBp{4, 3};

        // Logicos
        case TipoToken::OrOr: return InfixBp{5, 6};
        case TipoToken::AndAnd: return InfixBp{7, 8};

        // Igualdade e Relacionais
        case TipoToken::Eq:
        case TipoToken::NotEq: return InfixBp{9, 10};
        case TipoToken::Lt:
        case TipoToken::Le:
        case TipoToken::Gt:
        case TipoToken::Ge: return InfixBp{11, 12};

        // Deslocamento / Fluxo E/S (<< e >>)
        case TipoToken::Shl:
        case TipoToken::Shr: return InfixBp{13, 14};

        // Aritmeticos
        case TipoToken::Plus:
        case TipoToken::Min: return InfixBp{15, 16};
        case TipoToken::Star:
        case TipoToken::Div:
        case TipoToken::Percent: return InfixBp{17, 18};

        default: return std::nullopt;
    }
}

bool is_postfix(TipoToken tipo) {
    return tipo == TipoToken::ColEsquerda || tipo == TipoToken::ColchEsq ||
           tipo == TipoToken::PlusPLus || tipo == TipoToken::MinMin;
}

bool is_lvalue(const Expr& e) {
    return std::holds_alternative(e.node) || std::holds_alternative(e.node);
}

std::string describe(const Token& t) {
    if (t.tipo == TipoToken::FimArquivo) return "fim do arquivo";
    return "'" + t.lexeme + "'";
}

ExprPtr build_binary(const Token& op, ExprPtr lhs, ExprPtr rhs) {
    BinaryOp bop;
    switch (op.tipo) {
        case TipoToken::Plus: bop = BinaryOp::Add; break;
        case TipoToken::Min: bop = BinaryOp::Sub; break;
        case TipoToken::Star: bop = BinaryOp::Mul; break;
        case TipoToken::Div: bop = BinaryOp::Div; break;
        case TipoToken::Percent: bop = BinaryOp::Mod; break;
        case TipoToken::Eq: bop = BinaryOp::Eq; break;
        case TipoToken::NotEq: bop = BinaryOp::Ne; break;
        case TipoToken::Lt: bop = BinaryOp::Lt; break;
        case TipoToken::Le: bop = BinaryOp::Le; break;
        case TipoToken::Gt: bop = BinaryOp::Gt; break;
        case TipoToken::Ge: bop = BinaryOp::Ge; break;
        case TipoToken::AndAnd: bop = BinaryOp::And; break;
        case TipoToken::OrOr: bop = BinaryOp::Or; break;
        case TipoToken::Shl: bop = BinaryOp::Shl; break;
        case TipoToken::Shr: bop = BinaryOp::Shr; break;
        default: throw CompileError("Operador binario desconhecido", op.posicao);
    }
    Posicao pos = lhs->posicao;
    return make_expr(pos, Binary{bop, std::move(lhs), std::move(rhs)});
}

ExprPtr build_assign(const Token& op, ExprPtr target, ExprPtr value) {
    if (!is_lvalue(*target)) {
        throw CompileError("O lado esquerdo da atribuicao precisa ser uma variavel ou elemento de array",
                           op.posicao);
    }
    AssignOp aop = op.tipo == TipoToken::Assign     ? AssignOp::Assign
                 : op.tipo == TipoToken::PlusAssign ? AssignOp::AddAssign
                                                    : AssignOp::SubAssign;
    Posicao pos = target->posicao;
    return make_expr(pos, Assign{aop, std::move(target), std::move(value)});
}

}  // namespace

Parser::Parser(std::vector tokens) : tokens_(std::move(tokens)) {
    // Garante o sentinela FimArquivo no final do vetor
    if (tokens_.empty() || tokens_.back().tipo != TipoToken::FimArquivo) {
        Posicao p = tokens_.empty() ? Posicao{} : tokens_.back().posicao;
        tokens_.push_back(Token{TipoToken::FimArquivo, "", p});
    }
}

const Token& Parser::advance() {
    const Token& t = tokens_[pos_];
    if (t.tipo != TipoToken::FimArquivo) ++pos_;
    return t;
}

const Token& Parser::expect(TipoToken tipo) {
    if (!check(tipo)) {
        throw CompileError("Token inesperado encontrado " + describe(current()),
                           current().posicao);
    }
    return advance();
}

ExprPtr Parser::parse_expression() { return parse_expr_bp(0); }

// Coracao do Pratt parser: le operando e aplica operadores de acordo com a precedencia
ExprPtr Parser::parse_expr_bp(int min_bp) {
    ExprPtr lhs = parse_prefix();
    for (;;) {
        TipoToken k = current().tipo;

        if (is_postfix(k)) {
            if (kPostfixBp < min_bp) break;
            lhs = parse_postfix(std::move(lhs));
            continue;
        }

        auto bp = infix_bp(k);
        if (!bp || bp->left < min_bp) break;

        Token op = advance();

        // Tratamento especial para o Operador Ternario (cond ? expr1 : expr2)
        if (op.tipo == TipoToken::Interrogacao) {
            ExprPtr then_branch = parse_expression();
            expect(TipoToken::DoisPontos);
            ExprPtr else_branch = parse_expr_bp(bp->right);
            Posicao pos = lhs->posicao;
            lhs = make_expr(pos, Ternary{std::move(lhs), std::move(then_branch), std::move(else_branch)});
            continue;
        }

        ExprPtr rhs = parse_expr_bp(bp->right);
        if (op.tipo == TipoToken::Assign || op.tipo == TipoToken::PlusAssign ||
            op.tipo == TipoToken::MinAssign) {
            lhs = build_assign(op, std::move(lhs), std::move(rhs));
        } else {
            lhs = build_binary(op, std::move(lhs), std::move(rhs));
        }
    }
    return lhs;
}

ExprPtr Parser::parse_prefix() {
    Token t = advance();
    switch (t.tipo) {
        case TipoToken::IntLit: {
            try {
                return make_expr(t.posicao, IntLit{std::stoll(t.lexeme)});
            } catch (const std::out_of_range&) {
                throw CompileError("Literal inteiro fora do intervalo", t.posicao);
            }
        }
        case TipoToken::DoubleLit: {
            try {
                return make_expr(t.posicao, DoubleLit{std::stod(t.lexeme)});
            } catch (const std::out_of_range&) {
                throw CompileError("Literal double fora do intervalo", t.posicao);
            }
        }
        case TipoToken::CharLit: {
            // Remove as aspas simples do char (ex: "'a'" -> 'a')
            char c = (t.lexeme.size() >= 3) ? t.lexeme[1] : '\0';
            return make_expr(t.posicao, CharLit{c});
        }
        case TipoToken::StringLit: 
            return make_expr(t.posicao, StringLit{t.lexeme});
        
        case TipoToken::PalTrue:  
            return make_expr(t.posicao, BoolLit{true});
        
        case TipoToken::PalFalse: 
            return make_expr(t.posicao, BoolLit{false});
        
        case TipoToken::Ident:    
            return make_expr(t.posicao, Ident{t.lexeme});

        case TipoToken::ColEsquerda: {
            ExprPtr inner = parse_expr_bp(0);
            expect(TipoToken::ColDireita);
            return inner;
        }

        case TipoToken::Min:
            return make_expr(t.posicao, Unary{UnaryOp::Neg, parse_expr_bp(kPrefixBp)});
        
        case TipoToken::Not:
            return make_expr(t.posicao, Unary{UnaryOp::Not, parse_expr_bp(kPrefixBp)});
        
        case TipoToken::PlusPLus:
        case TipoToken::MinMin: {
            ExprPtr operand = parse_expr_bp(kPrefixBp);
            if (!is_lvalue(*operand)) {
                throw CompileError("O operando de incremento/decremento precisa ser variavel ou elemento de array",
                                   t.posicao);
            }
            UnaryOp op = t.tipo == TipoToken::PlusPLus ? UnaryOp::PreInc : UnaryOp::PreDec;
            return make_expr(t.posicao, Unary{op, std::move(operand)});
        }

        default:
            throw CompileError("Expressao esperada mas foi encontrado " + describe(t), t.posicao);
    }
}

ExprPtr Parser::parse_postfix(ExprPtr lhs) {
    Token t = advance();
    Posicao pos = lhs->posicao;
    switch (t.tipo) {
        case TipoToken::ColEsquerda: { // Chamada de funcao f(...)
            auto* id = std::get_if(&lhs->node);
            if (!id) throw CompileError("So e possivel chamar funcoes diretamente pelo nome", t.posicao);
            
            std::vector args;
            if (!check(TipoToken::ColDireita)) {
                args.push_back(parse_expr_bp(0));
                while (check(TipoToken::Virg)) {
                    advance(); // consome ','
                    args.push_back(parse_expr_bp(0));
                }
            }
            expect(TipoToken::ColDireita);
            return make_expr(pos, Call{id->name, std::move(args)});
        }
        case TipoToken::ColchEsq: { // Acesso a array arr[i]
            ExprPtr index = parse_expr_bp(0);
            expect(TipoToken::ColchDir);
            return make_expr(pos, Index{std::move(lhs), std::move(index)});
        }
        case TipoToken::PlusPLus:
        case TipoToken::MinMin: { // Pos-incremento e pos-decremento
            if (!is_lvalue(*lhs)) {
                throw CompileError("O operando precisa ser uma variavel ou elemento de array", t.posicao);
            }
            UnaryOp op = t.tipo == TipoToken::PlusPLus ? UnaryOp::PostInc : UnaryOp::PostDec;
            return make_expr(pos, Unary{op, std::move(lhs)});
        }
        default:
            throw CompileError("Operador pos-fixo inesperado", t.posicao);
    }
}

ExprPtr parse_expression_string(const std::string& source) {
    // Assume que a classe Lexer retorna std::vector com a estrutura do token.hpp
    Parser parser(Lexer(source).tokenize());
    ExprPtr expr = parser.parse_expression();
    if (!parser.at_end()) {
        throw CompileError("Token inesperado apos expressao " + describe(parser.current()),
                           parser.current().posicao);
    }
    return expr;
}

}
