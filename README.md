# MiniC++

Compilador/interpretador de um subconjunto de C++, escrito em C++17.
Projeto da disciplina de Compiladores, baseado no MiniC do professor.

## Como compilar e testar

Precisa de um compilador C++17 (g++ 9+, clang 10+ ou Visual Studio 2019+) e CMake 3.16+.
No Windows, o caminho mais tranquilo é o WSL2 (Ubuntu) com `sudo apt install build-essential cmake`.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure    # roda os testes
```

Sem CMake (só para conferir que compila, ajuste a lista de .cpp conforme o CMakeLists.txt):

```bash
g++ -std=c++17 -Wall -Wextra -Isrc -Itests \
    src/lexer.cpp src/ast_printer.cpp src/parser/*.cpp \
    tests/test_main.cpp tests/test_lexer.cpp tests/test_parser_*.cpp \
    -o minicpp_tests && ./minicpp_tests
```

## Usando

```bash
./build/minicpp --expr "1 + 2 * 3"            # (+ 1 (* 2 3))
./build/minicpp --expr "cout << a + b"        # (<< cout (+ a b))
./build/minicpp --tokens examples/valid/fatorial.mcpp
```

## Estrutura

```
minicpp/
├── CMakeLists.txt
├── README.md
├── docs/
│   └── grammar.md            # A GRAMÁTICA da linguagem (EBNF) — entregável da entrega 2
├── src/
│   ├── token.hpp              # tokens, posição (linha:coluna), CompileError
│   ├── lexer.{hpp,cpp}        # texto -> tokens
│   ├── ast.hpp                # nós da AST (std::variant + unique_ptr)
│   ├── ast_printer.{hpp,cpp}  # AST -> S-expression, usado nos testes
│   ├── parser/
│   │   ├── parser.hpp         # declara a classe Parser (única para todo o grupo)
│   │   ├── expressions.cpp    # ✅ pronto: 1+2*3, cout<<x, chamadas, a[i], ++/--, =/+=
│   │   ├── types.cpp          # 🚧 TODO: int, double, auto, const T, T&
│   │   ├── declarations.cpp   # 🚧 TODO: int x = 5;
│   │   ├── statements.cpp     # 🚧 TODO: if, while, for, break, continue, return, bloco
│   │   ├── functions.cpp      # 🚧 TODO: uma função inteira
│   │   └── program.cpp        # 🚧 TODO: várias funções = um programa
│   └── main.cpp                # --expr "..." e --tokens arquivo
├── examples/
│   ├── valid/                 # programas que DEVEM ser aceitos
│   └── invalid/               # programas que DEVEM dar erro
└── tests/
    ├── mini_test.hpp           # mini-framework de testes, sem dependências
    ├── test_lexer.cpp
    ├── test_parser_expressions.cpp  # ✅ 52 testes já passando
    ├── test_parser_statements.cpp   # 🚧 TODO
    ├── test_parser_functions.cpp    # 🚧 TODO
    └── test_parser_program.cpp      # 🚧 TODO
```

**Por que um .cpp por assunto dentro de `src/parser/`?** Todos implementam
métodos da MESMA classe `Parser` (declarada uma vez em `parser.hpp`), só que
cada um num arquivo separado. Assim cada pessoa do grupo mexe no seu arquivo
sem entrar em conflito de Git com as outras. Isso é só uma questão de
organização — o compilador junta tudo na hora de gerar o executável.

## O que já funciona

- Lexer completo: literais (`int`, `double`, `string` com escapes), identificadores, palavras-chave,
  todos os operadores, comentários `//` e `/* */`, erros com linha e coluna.
- Parser de **expressões** completo (`src/parser/expressions.cpp`): precedência e associatividade
  como em C++, `cout << ...`, chamadas, indexação, `++`/`--` (pré e pós), atribuição e `+=`/`-=`.
- 52 testes em `test_parser_expressions.cpp` + `test_lexer.cpp`, todos passando (inclusive sob
  AddressSanitizer/UBSan e um fuzz simples de 200 mil entradas aleatórias sem crash).

## Como dividir o trabalho que falta (entrega 2)

Cada arquivo TODO em `src/parser/` tem instruções detalhadas no topo, explicando o que
implementar e dando dicas. Sugestão de divisão por pessoa:

| Quem | Arquivo | O que faz |
|---|---|---|
| Pessoa 1 | `types.cpp` + `declarations.cpp` | tipos e `int x = 5;` |
| Pessoa 2 | `statements.cpp` (parte 1) | bloco, `if`/`else`, `while` |
| Pessoa 3 | `statements.cpp` (parte 2) | `for`, `break`, `continue`, `return` |
| Pessoa 4 | `functions.cpp` + `program.cpp` | função completa e programa completo (depende dos outros) |

Ordem de dependência: `types.cpp` → `declarations.cpp` e `statements.cpp` → `functions.cpp` →
`program.cpp`. Ou seja, comecem por tipos.

Cada método implementado precisa:
1. Ter o corpo escrito no `.cpp` correspondente.
2. Ter a declaração "descomentada" em `src/parser/parser.hpp` (hoje está como comentário/TODO).
3. Ter testes no arquivo correspondente em `tests/`.
4. Se mudar alguma regra da gramática, atualizar `docs/grammar.md` também.

Depois que tudo isso estiver pronto, adicionem `--ast <arquivo>` em `main.cpp` para imprimir a
AST de um programa completo (hoje só existe `--expr` para uma expressão solta).

## Decisões de sintaxe em aberto

Registrem a decisão em `docs/grammar.md` (seção "Histórico de decisões") quando resolverem:

- `for (int i = 0; ...)`: como distinguir declaração de expressão no primeiro campo.
- `else if`: tratar como `else` seguido de um `if` (mais simples), ou dar uma regra própria?
- `T&` só em parâmetros, ou também em variáveis locais?
- `class`/orientação a objetos (ver `docs/grammar.md`, seção "Classes"): entra nesta entrega
  ou fica para a entrega 3? Recomendação: classes simples + encapsulamento entram; herança só
  se sobrar tempo; `virtual`/polimorfismo fica fora do escopo do projeto.

## Convenções

- Todo erro é um `CompileError` com `Pos` (linha:coluna). Nada de `exit()` no meio do compilador.
- Todo arquivo `.cpp` novo entra na lista do `add_library`/`add_executable` no `CMakeLists.txt`.
- Testes novos: `TEST(nome) { CHECK_EQ(...); CHECK_ERROR(expr, linha, coluna); }` (ver `tests/mini_test.hpp`).
- Para cada programa de exemplo válido em `examples/valid/`, tentem também escrever uma variação
  quebrada em `examples/invalid/` — ajuda a pensar nos casos de erro que os testes precisam cobrir.
