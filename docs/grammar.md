# Gramática do MiniC++

Notação EBNF. `:=` define uma regra, `|` é alternativa, `[...]` é opcional,
`(...)* ` é "zero ou mais", `(...)+ ` é "uma ou mais". Literais entre aspas
são tokens exatos (`'if'`, `'('`).

Legenda de status: ✅ implementado no parser · 🚧 planejado para esta entrega
· 🔵 extensão (orientação a objetos), fora do escopo obrigatório da entrega 2.

---

## Programa

```ebnf
program    := (function | class_decl)*        🚧 (só function por enquanto)
```

Um programa é uma sequência de funções (e, futuramente, classes) até o fim
do arquivo. Diferente do MiniC original, um erro de sintaxe no meio do
arquivo **não pode** ser silenciosamente ignorado — o parser precisa
consumir toda a entrada ou lançar erro.

## Tipos

```ebnf
type       := base_type | 'const' type | type '&'  ✅
base_type  := 'int' | 'double' | 'bool' | 'string' | 'void' | 'auto'
```

- `T&` só é permitido em parâmetro de função (referência), não em variável
  local. Decisão do grupo: registrar aqui se isso mudar.
- `auto` exige inicializador (`auto x = 5;`); a checagem em si é do type
  checker (entrega 3), o parser só reconhece a palavra.
- `const` vem antes do tipo: `const int x`. `&` vem depois: `int& x`.

## Funções

```ebnf
function   := type IDENT '(' [param (',' param)*] ')' block   🚧
param      := type IDENT                                       🚧
```

Exemplo:
```cpp
int fatorial(int n) { ... }
void incrementa(int& x) { ... }
```

## Comandos (statements)

```ebnf
statement  := block | if_stmt | while_stmt | for_stmt
            | 'break' ';' | 'continue' ';' | return_stmt
            | var_decl ';' | expr_stmt                          🚧

block      := '{' statement* '}'                                ✅

if_stmt    := 'if' '(' expr ')' statement ['else' statement]    ✅

while_stmt := 'while' '(' expr ')' statement                    ✅

for_stmt   := 'for' '(' [for_init] ';' [expr] ';' [expr] ')' statement   🚧
for_init   := var_decl | expr

return_stmt := 'return' [expr] ';'                               🚧

var_decl   := type IDENT ['=' expr]                              ✅

expr_stmt  := expr ';'                                           🚧
```

Todo comando terminado em `;` é "simples"; `block`, `if`, `while`, `for`
não levam `;` no final (terminam em `}` ou no `statement` do corpo).

## Expressões — ✅ já implementado

Da mais fraca para a mais forte precedência (igual ao C++):

```ebnf
expr        := assignment                                        ✅
assignment  := lvalue ('=' | '+=' | '-=') assignment | logic_or   ✅
logic_or    := logic_and ('||' logic_and)*                        ✅
logic_and   := equality ('&&' equality)*                          ✅
equality    := relational (('==' | '!=') relational)*             ✅
relational  := shift (('<' | '<=' | '>' | '>=') shift)*           ✅
shift       := additive ('<<' additive)*                          ✅
additive    := term (('+' | '-') term)*                           ✅
term        := unary (('*' | '/' | '%') unary)*                   ✅
unary       := ('-' | '!' | '++' | '--') unary | postfix           ✅
postfix     := primary ('[' expr ']' | '++' | '--')*               ✅
primary     := INT_LIT | DOUBLE_LIT | STRING_LIT | 'true' | 'false'
             | IDENT ['(' [expr (',' expr)*] ')']
             | '(' expr ')'                                        ✅

lvalue      := IDENT ('[' expr ']')*                               ✅
```

Notas de implementação (ver `src/parser/expressions.cpp`):
- `!` tem precedência **alta** (nível de `unary`), diferente do MiniC
  original onde `!` ficava entre `and` e os relacionais.
- `=`, `+=`, `-=` são associativos à direita (`a = b = c` = `a = (b = c)`);
  todo o resto é associativo à esquerda.
- `cout << x << endl` funciona porque `<<` é só mais um operador binário
  (nível `shift`); `cout` e `endl` são identificadores comuns — quem dá
  significado especial a eles é o type checker/interpretador, não o parser.

## Classes — 🔵 extensão, fora do escopo mínimo da entrega 2

Registrado aqui como referência para quando o grupo decidir avançar (fica
bem no início da entrega 3, já que depende do type checker para
encapsulamento):

```ebnf
class_decl  := 'class' IDENT [':' access_spec IDENT] '{' member* '}' ';'
member      := access_spec ':' | field_decl | method_decl | ctor_decl
access_spec := 'public' | 'private' | 'protected'
field_decl  := var_decl ';'
method_decl := function
ctor_decl   := IDENT '(' [param (',' param)*] ')' block
```

Fora de escopo (ver discussão no grupo): `virtual` e despacho dinâmico
(polimorfismo) — custo muito alto (vtables) para o tempo disponível.

---

## Histórico de decisões

*(Preencham conforme forem decidindo algo que vale registrar — ex.: "decidimos
que `else if` é tratado como `else` seguido de outro `if`, sem regra própria
na gramática".)*
