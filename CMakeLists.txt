cmake_minimum_required(VERSION 3.16)
project(minicpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Biblioteca com o compilador (lexer, parser, AST). O executavel e os testes usam ela.
# Ao criar um .cpp novo em src/ ou src/parser/, adicione-o aqui.
add_library(minicpp_lib STATIC
    src/lexer.cpp
    src/ast_printer.cpp
    src/parser/expressions.cpp
    src/parser/types.cpp
    src/parser/declarations.cpp
    src/parser/statements.cpp
    src/parser/functions.cpp
    src/parser/program.cpp
)
target_include_directories(minicpp_lib PUBLIC src)

if(MSVC)
    target_compile_options(minicpp_lib PUBLIC /W4)
else()
    target_compile_options(minicpp_lib PUBLIC -Wall -Wextra -Wpedantic)
endif()

# Executavel: build/minicpp
add_executable(minicpp src/main.cpp)
target_link_libraries(minicpp PRIVATE minicpp_lib)

# Testes: build/minicpp_tests   (rodar com: ctest --test-dir build --output-on-failure)
enable_testing()
add_executable(minicpp_tests
    tests/test_main.cpp
    tests/test_lexer.cpp
    tests/test_parser_expressions.cpp
    tests/test_parser_statements.cpp
    tests/test_parser_functions.cpp
    tests/test_parser_program.cpp
)
target_include_directories(minicpp_tests PRIVATE tests)
target_link_libraries(minicpp_tests PRIVATE minicpp_lib)
add_test(NAME minicpp_tests COMMAND minicpp_tests)
