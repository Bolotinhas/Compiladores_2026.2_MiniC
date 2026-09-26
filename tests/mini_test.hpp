// mini_test.hpp - mini-framework de testes (sem dependencias externas).
//
// Uso:
//   TEST(nome_do_teste) {
//       CHECK(1 + 1 == 2);
//       CHECK_EQ(to_sexpr(*parse_expression_string("1+2")), "(+ 1 2)");
//       CHECK_ERROR(parse_expression_string("1 +"), 1, 4);   // linha, coluna
//   }
//
// Se preferirem Catch2 ou GoogleTest depois, as macros TEST/CHECK_EQ tem
// equivalentes diretos (TEST_CASE / REQUIRE).
#pragma once

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "token.hpp"

namespace mt {

struct Failure {
    std::string message;
};

struct Case {
    const char* name;
    std::function<void()> fn;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> cases;
    return cases;
}

struct Registrar {
    Registrar(const char* name, std::function<void()> fn) { registry().push_back({name, fn}); }
};

inline std::string where(const char* file, int line) {
    return std::string(file) + ":" + std::to_string(line) + ": ";
}

}  // namespace mt

#define TEST(name)                                          \
    static void name();                                     \
    static mt::Registrar registrar_##name(#name, name);     \
    static void name()

#define CHECK(cond)                                                                   \
    do {                                                                              \
        if (!(cond)) throw mt::Failure{mt::where(__FILE__, __LINE__) + "CHECK falhou: " #cond}; \
    } while (0)

#define CHECK_EQ(actual, expected)                                                    \
    do {                                                                              \
        auto a_ = (actual);                                                           \
        auto e_ = (expected);                                                         \
        if (!(a_ == e_)) {                                                            \
            std::ostringstream os_;                                                   \
            os_ << mt::where(__FILE__, __LINE__) << "esperado [" << e_                \
                << "] mas obteve [" << a_ << "]";                                     \
            throw mt::Failure{os_.str()};                                             \
        }                                                                             \
    } while (0)

// Verifica que a expressao lanca CompileError exatamente em (line, col).
#define CHECK_ERROR(expr, line_, col_)                                                \
    do {                                                                              \
        bool threw_ = false;                                                          \
        try {                                                                         \
            (void)(expr);                                                             \
        } catch (const minicpp::CompileError& err_) {                                 \
            threw_ = true;                                                            \
            if (err_.pos().line != (line_) || err_.pos().col != (col_)) {             \
                std::ostringstream os_;                                               \
                os_ << mt::where(__FILE__, __LINE__) << "erro na posicao "            \
                    << err_.pos().line << ":" << err_.pos().col << " (esperado "      \
                    << (line_) << ":" << (col_) << "): " << err_.message();           \
                throw mt::Failure{os_.str()};                                         \
            }                                                                         \
        }                                                                             \
        if (!threw_)                                                                  \
            throw mt::Failure{mt::where(__FILE__, __LINE__) + "esperava CompileError: " #expr}; \
    } while (0)
