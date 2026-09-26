#include <vector>

#include "lexer.hpp"
#include "mini_test.hpp"

using namespace minicpp;
using K = TokenKind;

static std::vector<Token> lex(const std::string& src) { return Lexer(src).tokenize(); }

static std::vector<K> kinds(const std::string& src) {
    std::vector<K> out;
    for (const auto& t : lex(src)) out.push_back(t.kind);
    return out;
}

TEST(lexer_declaracao_simples) {
    std::vector<K> expected = {K::KwInt, K::Ident, K::Assign, K::IntLit, K::Semicolon, K::Eof};
    CHECK(kinds("int x = 42;") == expected);
}

TEST(lexer_entrada_vazia_so_tem_eof) {
    auto toks = lex("");
    CHECK_EQ(toks.size(), std::size_t{1});
    CHECK(toks[0].kind == K::Eof);
}

TEST(lexer_palavra_chave_vs_identificador) {
    // 'integer' comeca com 'int' mas e um identificador
    std::vector<K> expected = {K::KwInt, K::Ident, K::KwIf, K::Ident, K::Eof};
    CHECK(kinds("int integer if iffy") == expected);
}

TEST(lexer_cout_e_endl_sao_identificadores) {
    std::vector<K> expected = {K::Ident, K::Shl, K::Ident, K::Eof};
    CHECK(kinds("cout << endl") == expected);
}

TEST(lexer_operadores_compostos) {
    std::vector<K> expected = {K::Le, K::Shl, K::PlusPlus, K::PlusAssign, K::AndAnd,
                               K::OrOr, K::EqEq, K::NotEq, K::MinusMinus, K::MinusAssign,
                               K::Ge, K::Eof};
    CHECK(kinds("<= << ++ += && || == != -- -= >=") == expected);
}

TEST(lexer_operadores_simples_e_pontuacao) {
    std::vector<K> expected = {K::Plus, K::Minus, K::Star, K::Slash, K::Percent, K::Lt, K::Gt,
                               K::Not, K::Amp, K::Assign, K::LParen, K::RParen, K::LBrace,
                               K::RBrace, K::LBracket, K::RBracket, K::Comma, K::Semicolon,
                               K::Eof};
    CHECK(kinds("+ - * / % < > ! & = ( ) { } [ ] , ;") == expected);
}

TEST(lexer_maximal_munch_sem_espacos) {
    // a+++b deve virar a ++ + b (como no C++)
    std::vector<K> expected = {K::Ident, K::PlusPlus, K::Plus, K::Ident, K::Eof};
    CHECK(kinds("a+++b") == expected);
}

TEST(lexer_numeros) {
    auto toks = lex("3 3.14 0.5");
    CHECK(toks[0].kind == K::IntLit);
    CHECK_EQ(toks[0].lexeme, "3");
    CHECK(toks[1].kind == K::DoubleLit);
    CHECK_EQ(toks[1].lexeme, "3.14");
    CHECK(toks[2].kind == K::DoubleLit);
}

TEST(lexer_numero_mal_formado) { CHECK_ERROR(lex("12abc"), 1, 1); }

TEST(lexer_string_com_escapes) {
    auto toks = lex("\"a\\nb\\t\\\"c\\\\\"");
    CHECK(toks[0].kind == K::StringLit);
    CHECK_EQ(toks[0].lexeme, "a\nb\t\"c\\");
}

TEST(lexer_string_vazia) {
    auto toks = lex("\"\"");
    CHECK(toks[0].kind == K::StringLit);
    CHECK_EQ(toks[0].lexeme, "");
}

TEST(lexer_string_nao_terminada) { CHECK_ERROR(lex("x = \"abc"), 1, 5); }
TEST(lexer_string_quebra_de_linha) { CHECK_ERROR(lex("\"abc\ndef\""), 1, 1); }
TEST(lexer_escape_invalido) { CHECK_ERROR(lex("\"a\\qb\""), 1, 3); }

TEST(lexer_comentarios_sao_ignorados) {
    std::vector<K> expected = {K::Ident, K::Ident, K::Ident, K::Eof};
    CHECK(kinds("a // linha inteira\n b /* bloco\n de varias linhas */ c") == expected);
}

TEST(lexer_comentario_de_bloco_nao_terminado) { CHECK_ERROR(lex("a /* nunca fecha"), 1, 3); }

TEST(lexer_linha_e_coluna) {
    auto toks = lex("a\n  bb\n\tc");
    CHECK_EQ(toks[0].pos.line, 1);
    CHECK_EQ(toks[0].pos.col, 1);
    CHECK_EQ(toks[1].pos.line, 2);
    CHECK_EQ(toks[1].pos.col, 3);
    CHECK_EQ(toks[2].pos.line, 3);
    CHECK_EQ(toks[2].pos.col, 2);
}

TEST(lexer_caractere_invalido) { CHECK_ERROR(lex("int x = @;"), 1, 9); }
TEST(lexer_barra_vertical_sozinha) { CHECK_ERROR(lex("a | b"), 1, 3); }
