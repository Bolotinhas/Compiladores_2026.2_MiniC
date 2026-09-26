// main.cpp - ferramenta de linha de comando do MiniC++.
//
//   minicpp --tokens <arquivo>     lista os tokens do arquivo
//   minicpp --expr "<expressao>"   faz o parse de UMA expressao e imprime a AST
//
// TODO(grupo): quando o parser de programas existir, adicionar
//   minicpp --ast <arquivo>        imprime a AST do programa
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "ast_printer.hpp"
#include "lexer.hpp"
#include "parser/parser.hpp"

namespace {

int usage() {
    std::cerr << "Uso:\n"
                 "  minicpp --tokens <arquivo>\n"
                 "  minicpp --expr \"<expressao>\"\n";
    return 2;
}

bool read_file(const std::string& path, std::string& out) {
    std::ifstream in(path);
    if (!in) return false;
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) return usage();
    const std::string flag = argv[1];
    const std::string arg = argv[2];

    try {
        if (flag == "--tokens") {
            std::string source;
            if (!read_file(arg, source)) {
                std::cerr << "erro: nao foi possivel abrir '" << arg << "'\n";
                return 1;
            }
            for (const auto& t : minicpp::Lexer(source).tokenize()) {
                std::cout << t.pos.line << ":" << t.pos.col << "\t"
                          << minicpp::to_string(t.kind);
                if (!t.lexeme.empty()) std::cout << "\t" << t.lexeme;
                std::cout << "\n";
            }
            return 0;
        }
        if (flag == "--expr") {
            auto expr = minicpp::parse_expression_string(arg);
            std::cout << minicpp::to_sexpr(*expr) << "\n";
            return 0;
        }
        return usage();
    } catch (const minicpp::CompileError& e) {
        std::cerr << "erro: " << e.what() << "\n";
        return 1;
    }
}
