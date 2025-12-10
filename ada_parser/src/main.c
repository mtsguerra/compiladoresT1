#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "semantic.h"
#include "symbol_table.h"
#include "mips_codegen.h"

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
        printf("Usage: %s <ada_file> [-o output_file]\n", argv[0]);
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

    // Parse command line arguments
    const char *input_file = argv[1];
    const char *output_file = "output.asm";
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[i + 1];
            i++;
        }
    }

    char *source = read_file(input_file);
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

    if (!ast) {
        fprintf(stderr, "Error: Failed to parse input file\n");
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    printf("\n=== Abstract Syntax Tree ===\n");
    ast_print(ast, 0);

    // 3. Semantic Analysis
    printf("\n=== Semantic Analysis ===\n");
    SemanticContext *semantic_ctx = semantic_context_create();
    semantic_analyze(ast, semantic_ctx);
    
    if (semantic_ctx->error_count > 0) {
        fprintf(stderr, "\nCompilation failed due to semantic errors\n");
        semantic_context_free(semantic_ctx);
        ast_free(ast);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    // 4. MIPS Code Generation
    printf("\n=== MIPS Code Generation ===\n");
    FILE *output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        semantic_context_free(semantic_ctx);
        ast_free(ast);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    MIPSCodeGen *codegen = mips_codegen_create(output, semantic_ctx->current_scope);
    mips_codegen_generate(codegen, ast);
    fclose(output);
    
    printf("Compilation successful! Output written to: %s\n", output_file);
    printf("\nTo run the generated code:\n");
    printf("  spim -file %s\n", output_file);
    printf("or\n");
    printf("  java -jar mars.jar %s\n", output_file);

    // Cleanup
    mips_codegen_free(codegen);
    semantic_context_free(semantic_ctx);
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);

    return 0;
}