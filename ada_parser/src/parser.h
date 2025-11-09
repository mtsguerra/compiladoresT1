#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer *lexer;
    Token *current_token;
} Parser;

// Protótipos das funções
Parser* parser_create(Lexer *lexer);
void parser_free(Parser *parser);
ASTNode* parser_parse(Parser *parser);

#endif