// test_parser_expressions.cpp - testes do parser de EXPRESSOES (ja implementado).
// Para os comandos, funcoes e programa (ainda TODO), ver os arquivos
// test_parser_statements.cpp, test_parser_functions.cpp e test_parser_program.cpp.

#include "ast_printer.hpp"
#include "mini_test.hpp"
#include "parser/parser.hpp"

using namespace minicpp;

// Faz o parse de uma expressao e devolve a AST como S-expression.
static std::string sx(const std::string& src) { return to_sexpr(*parse_expression_string(src)); }

// ------------------------------------------------------------ o primeiro teste

TEST(primeiro_teste_precedencia_mul_sobre_soma) { CHECK_EQ(sx("1 + 2 * 3"), "(+ 1 (* 2 3))"); }

// ------------------------------------------------------------------- literais

TEST(parser_literais) {
    CHECK_EQ(sx("42"), "42");
    CHECK_EQ(sx("1.5"), "1.5");
    CHECK_EQ(sx("2.0"), "2.0");
    CHECK_EQ(sx("true"), "true");
    CHECK_EQ(sx("false"), "false");
    CHECK_EQ(sx("\"oi\""), "\"oi\"");
    CHECK_EQ(sx("x"), "x");
}

// ----------------------------------------------- precedencia e associatividade

TEST(parser_parenteses_mudam_a_precedencia) {
    CHECK_EQ(sx("(1 + 2) * 3"), "(* (+ 1 2) 3)");
    CHECK_EQ(sx("((1))"), "1");
}

TEST(parser_soma_e_subtracao_associam_a_esquerda) {
    CHECK_EQ(sx("a - b - c"), "(- (- a b) c)");
    CHECK_EQ(sx("a - b + c"), "(+ (- a b) c)");
}

TEST(parser_mul_div_mod_associam_a_esquerda) {
    CHECK_EQ(sx("a / b * c"), "(* (/ a b) c)");
    CHECK_EQ(sx("a * b % c"), "(% (* a b) c)");
}

TEST(parser_atribuicao_associa_a_direita) {
    CHECK_EQ(sx("a = b = c"), "(= a (= b c))");
    CHECK_EQ(sx("a = b + 1"), "(= a (+ b 1))");
}

TEST(parser_atribuicao_composta) {
    CHECK_EQ(sx("x += 1 + 2"), "(+= x (+ 1 2))");
    CHECK_EQ(sx("x -= y"), "(-= x y)");
}

TEST(parser_precedencia_logica) {
    CHECK_EQ(sx("a || b && c"), "(|| a (&& b c))");
    CHECK_EQ(sx("a && b || c"), "(|| (&& a b) c)");
}

TEST(parser_relacional_liga_mais_forte_que_igualdade) {
    CHECK_EQ(sx("a < b == c < d"), "(== (< a b) (< c d))");
    CHECK_EQ(sx("a + 1 <= b * 2"), "(<= (+ a 1) (* b 2))");
}

TEST(parser_todos_os_relacionais) {
    CHECK_EQ(sx("a < b"), "(< a b)");
    CHECK_EQ(sx("a <= b"), "(<= a b)");
    CHECK_EQ(sx("a > b"), "(> a b)");
    CHECK_EQ(sx("a >= b"), "(>= a b)");
    CHECK_EQ(sx("a == b"), "(== a b)");
    CHECK_EQ(sx("a != b"), "(!= a b)");
}

// ------------------------------------------------------------------- unarios

TEST(parser_menos_unario) {
    CHECK_EQ(sx("-a * b"), "(* (neg a) b)");
    CHECK_EQ(sx("- -a"), "(neg (neg a))");
    CHECK_EQ(sx("1 - -2"), "(- 1 (neg 2))");
}

TEST(parser_not_liga_forte_como_no_cpp) {
    // Diferente do MiniC original: '!' tem precedencia alta, como em C++.
    CHECK_EQ(sx("!a && b"), "(&& (! a) b)");
    CHECK_EQ(sx("!a == b"), "(== (! a) b)");
    CHECK_EQ(sx("!!a"), "(! (! a))");
}

TEST(parser_incremento_e_decremento) {
    CHECK_EQ(sx("i++"), "(post++ i)");
    CHECK_EQ(sx("i--"), "(post-- i)");
    CHECK_EQ(sx("++i"), "(pre++ i)");
    CHECK_EQ(sx("--i"), "(pre-- i)");
    CHECK_EQ(sx("i++ + ++j"), "(+ (post++ i) (pre++ j))");
    CHECK_EQ(sx("-a++"), "(neg (post++ a))");
    CHECK_EQ(sx("a[0]++"), "(post++ (index a 0))");
}

// ---------------------------------------------------------- cout e o operador <<

TEST(parser_cout_encadeado) {
    CHECK_EQ(sx("cout << \"x=\" << x << endl"), "(<< (<< (<< cout \"x=\") x) endl)");
}

TEST(parser_shift_liga_mais_fraco_que_soma) {
    CHECK_EQ(sx("cout << a + b"), "(<< cout (+ a b))");
    CHECK_EQ(sx("cout << a * 2 << b"), "(<< (<< cout (* a 2)) b)");
}

// ---------------------------------------------------------- chamadas e indices

TEST(parser_chamadas) {
    CHECK_EQ(sx("f()"), "(call f)");
    CHECK_EQ(sx("f(1)"), "(call f 1)");
    CHECK_EQ(sx("f(1, g(2), 3 + 4)"), "(call f 1 (call g 2) (+ 3 4))");
    CHECK_EQ(sx("1 + f(2) * 3"), "(+ 1 (* (call f 2) 3))");
}

TEST(parser_indices) {
    CHECK_EQ(sx("a[0]"), "(index a 0)");
    CHECK_EQ(sx("a[i][j]"), "(index (index a i) j)");
    CHECK_EQ(sx("a[i + 1]"), "(index a (+ i 1))");
    CHECK_EQ(sx("f(1, g(2))[0]"), "(index (call f 1 (call g 2)) 0)");
    CHECK_EQ(sx("-a[0]"), "(neg (index a 0))");
}

TEST(parser_atribuicao_a_elemento_de_array) {
    CHECK_EQ(sx("a[i][j] = 3"), "(= (index (index a i) j) 3)");
}

// ------------------------------------------------------------ comentarios

TEST(parser_ignora_comentarios) {
    CHECK_EQ(sx("1 /* soma */ + // resto\n 2"), "(+ 1 2)");
}

// -------------------------------------------------------------------- erros

TEST(parser_erro_operando_faltando) { CHECK_ERROR(sx("1 +"), 1, 4); }
TEST(parser_erro_parentese_nao_fechado) { CHECK_ERROR(sx("(1 + 2"), 1, 7); }
TEST(parser_erro_parentese_sobrando) { CHECK_ERROR(sx("1 + 2)"), 1, 6); }
TEST(parser_erro_entrada_vazia) { CHECK_ERROR(sx(""), 1, 1); }
TEST(parser_erro_operador_no_inicio) { CHECK_ERROR(sx("* 2"), 1, 1); }
TEST(parser_erro_dois_operandos_seguidos) { CHECK_ERROR(sx("1 + 2 3"), 1, 7); }
TEST(parser_erro_colchete_nao_fechado) { CHECK_ERROR(sx("a[1"), 1, 4); }
TEST(parser_erro_virgula_sobrando_em_chamada) { CHECK_ERROR(sx("f(1,)"), 1, 5); }
TEST(parser_erro_erro_em_outra_linha) { CHECK_ERROR(sx("1 +\n\n  *"), 3, 3); }

TEST(parser_erro_atribuicao_a_literal) { CHECK_ERROR(sx("1 = 2"), 1, 3); }
TEST(parser_erro_atribuicao_a_expressao) { CHECK_ERROR(sx("a + b = 3"), 1, 7); }
TEST(parser_erro_incremento_de_literal) {
    CHECK_ERROR(sx("++1"), 1, 1);
    CHECK_ERROR(sx("1++"), 1, 2);
}
TEST(parser_erro_chamar_algo_que_nao_e_nome) {
    CHECK_ERROR(sx("f(1)(2)"), 1, 5);
    CHECK_ERROR(sx("1(2)"), 1, 2);
}

TEST(parser_parenteses_em_volta_do_nome_da_funcao_sao_validos) {
    // Como em C++: (f)(1) e uma chamada valida; o parentese nao deixa marca na AST.
    CHECK_EQ(sx("(f)(1)"), "(call f 1)");
}
