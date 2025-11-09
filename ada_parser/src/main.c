#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char*)malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);
    return content;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <ada_file>\n", argv[0]);
        printf("\nExample Ada program:\n");
        printf("procedure Main is\n");
        printf("begin\n");
        printf("  x := 10;\n");
        printf("  y := x + 5;\n");
        printf("  if y > 10 then\n");
        printf("    Put_Line(\"Greater than 10\");\n");
        printf("  else\n");
        printf("    Put_Line(\"Not greater\");\n");
        printf("  end if;\n");
        printf("end Main;\n");
        return 1;
    }

    char *source = read_file(argv[1]);
    if (!source) {
        return 1;
    }

    printf("=== Lexical Analysis ===\n");
    Lexer *lexer = lexer_create(source);
    Token *token;

    // Imprimir todos os tokens
    Lexer *lexer_copy = lexer_create(source);
    while ((token = lexer_next_token(lexer_copy))->type != TOKEN_EOF) {
        printf("%-15s : %s\n", token_type_to_string(token->type), token->value);
        token_free(token);
    }
    token_free(token);
    lexer_free(lexer_copy);

    printf("\n=== Syntactic Analysis ===\n");
    Parser *parser = parser_create(lexer);
    ASTNode *ast = parser_parse(parser);

    printf("\n=== Abstract Syntax Tree ===\n");
    ast_print(ast, 0);

    // Liberar memória
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);

    return 0;
}