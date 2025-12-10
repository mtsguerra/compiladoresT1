#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

// Contexto de análise semântica
typedef struct {
    SymbolTable *current_scope;
    int error_count;
} SemanticContext;

// Protótipos das funções
SemanticContext* semantic_context_create();
void semantic_context_free(SemanticContext *ctx);
void semantic_analyze(ASTNode *ast, SemanticContext *ctx);
void semantic_check_statement(ASTNode *node, SemanticContext *ctx);
SymbolType semantic_check_expression(ASTNode *expr, SemanticContext *ctx);
void semantic_error(const char *format, ...);

#endif
