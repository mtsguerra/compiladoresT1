#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Tipos de tokens
typedef enum {
    // Palavras-chaves da linguagem
    TOKEN_PROCEDURE,
    TOKEN_IS,
    TOKEN_BEGIN,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_LOOP,
    TOKEN_PUT_LINE,
    TOKEN_GET_LINE,

    // Indentificadores e literais
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_STRING,

    // Operadores e símbolos
    TOKEN_ASSIGN,       // :=
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_EQUAL,        // =
    TOKEN_NOT_EQUAL,    // /=
    TOKEN_LESS,         // <
    TOKEN_LESS_EQUAL,   // <=
    TOKEN_GREATER,      // >
    TOKEN_GREATER_EQUAL,// >=
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    // Delimitadores e pontuação
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_SEMICOLON,    // ;
    TOKEN_COLON,        // :
    TOKEN_COMMA,        // ,

    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

typedef struct {
    char *source;
    int position;
    int line;
    int column;
    char current_char;
} Lexer;

// Protótipos das funções
Lexer* lexer_create(const char *source);
void lexer_free(Lexer *lexer);
Token* lexer_next_token(Lexer *lexer);
void token_free(Token *token);
const char* token_type_to_string(TokenType type);

#endif