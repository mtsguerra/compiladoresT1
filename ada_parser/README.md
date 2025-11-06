# Ada Parser - Compilador Simplificado

Este projeto implementa um **scanner (análise léxica)** e **parser (análise sintática)** para um subconjunto simplificado da linguagem Ada.

## Características

### Análise Léxica (Scanner)
- Reconhecimento de palavras-chave Ada
- Identificadores e literais (inteiros e strings)
- Operadores aritméticos (+, -, *, /)
- Operadores relacionais (=, /=, <, <=, >, >=)
- Operadores lógicos (AND, OR, NOT)
- Comentários Ada (-- até fim de linha)
- Tratamento de espaços em branco

### Análise Sintática (Parser)
- Parsing de procedures
- Expressões aritméticas e booleanas
- Comandos de atribuição
- Estruturas condicionais (if-then-else)
- Loops (while)
- Funções de I/O (Put_Line, Get_Line)
- Geração de AST (Abstract Syntax Tree)

## Compilação

```bash
make
```

## Uso

```bash
./ada_parser exemplo.ada
```

## Exemplo de Programa Ada

```ada
procedure Main is
begin
  x := 10;
  y := x + 5;
  
  if y > 10 then
    Put_Line("Y é maior que 10");
  else
    Put_Line("Y não é maior que 10");
  end if;
  
  while x > 0 loop
    Put_Line("Contando...");
    x := x - 1;
  end loop;
  
  Get_Line(nome);
  Put_Line(nome);
end Main;
```

## Estrutura do Projeto

- **lexer.h/lexer.c** - Analisador léxico (scanner)
- **parser.h/parser.c** - Analisador sintático (parser)
- **ast.h/ast.c** - Estrutura da árvore sintática abstrata
- **main.c** - Programa principal
- **Makefile** - Script de compilação

## Gramática Suportada

```
program         ::= procedure
procedure       ::= 'procedure' IDENTIFIER 'is' 'begin' block 'end' [IDENTIFIER] ';'
block           ::= statement*
statement       ::= assignment | if_stmt | while_stmt | put_line | get_line
assignment      ::= IDENTIFIER ':=' expression ';'
if_stmt         ::= 'if' expression 'then' block ['else' block] 'end' 'if' ';'
while_stmt      ::= 'while' expression 'loop' block 'end' 'loop' ';'
put_line        ::= 'Put_Line' '(' expression ')' ';'
get_line        ::= 'Get_Line' '(' IDENTIFIER ')' ';'
expression      ::= or_expr
or_expr         ::= and_expr ('or' and_expr)*
and_expr        ::= rel_expr ('and' rel_expr)*
rel_expr        ::= add_expr (('<' | '<=' | '>' | '>=' | '=' | '/=') add_expr)*
add_expr        ::= mult_expr (('+' | '-') mult_expr)*
mult_expr       ::= primary (('*' | '/') primary)*
primary         ::= INTEGER | STRING | IDENTIFIER | '(' expression ')' | 'not' primary
```

## Funcionalidades

### ✅ Implementado
- Análise léxica completa
- Análise sintática com precedência de operadores
- Geração de AST
- Suporte a comentários
- Tratamento de erros com localização (linha/coluna)
- Expressões aritméticas e booleanas
- Estruturas de controle (if, while)
- I/O básico

### 🔄 Possíveis Extensões
- Análise semântica
- Geração de código intermediário
- Otimizações
- Suporte a funções e procedimentos
- Tipos de dados mais complexos
- Arrays e records

## Limpeza

```bash
make clean
```

## Autor

Projeto desenvolvido como trabalho de curso em Compiladores.