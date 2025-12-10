#define _GNU_SOURCE
#include "semantic.h"
#include <stdio.h>
#include <stdarg.h>

SemanticContext* semantic_context_create() {
    SemanticContext *ctx = (SemanticContext*)malloc(sizeof(SemanticContext));
    ctx->current_scope = symbol_table_create(NULL);
    ctx->error_count = 0;
    return ctx;
}

void semantic_context_free(SemanticContext *ctx) {
    if (!ctx) return;
    if (ctx->current_scope) {
        symbol_table_free(ctx->current_scope);
    }
    free(ctx);
}

void semantic_error(const char *format, ...) {
    fprintf(stderr, "Semantic error: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

SymbolType semantic_check_expression(ASTNode *expr, SemanticContext *ctx) {
    if (!expr) return SYMBOL_UNKNOWN;
    
    switch (expr->type) {
        case AST_INTEGER:
            return SYMBOL_INTEGER;
            
        case AST_STRING:
            return SYMBOL_STRING;
            
        case AST_IDENTIFIER: {
            Symbol *symbol = symbol_table_lookup(ctx->current_scope, expr->data.identifier.name);
            if (!symbol) {
                semantic_error("Variable '%s' used before declaration", expr->data.identifier.name);
                ctx->error_count++;
                return SYMBOL_UNKNOWN;
            }
            return symbol->type;
        }
            
        case AST_BINARY_OP: {
            SymbolType left_type = semantic_check_expression(expr->data.binary_op.left, ctx);
            SymbolType right_type = semantic_check_expression(expr->data.binary_op.right, ctx);
            
            const char *op = expr->data.binary_op.operator;
            
            // Operadores aritméticos requerem inteiros
            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || 
                strcmp(op, "*") == 0 || strcmp(op, "/") == 0) {
                if (left_type != SYMBOL_INTEGER || right_type != SYMBOL_INTEGER) {
                    semantic_error("Arithmetic operators require integer operands");
                    ctx->error_count++;
                    return SYMBOL_UNKNOWN;
                }
                return SYMBOL_INTEGER;
            }
            
            // Operadores relacionais retornam boolean
            if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || 
                strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
                strcmp(op, "=") == 0 || strcmp(op, "/=") == 0) {
                if (left_type != right_type) {
                    semantic_error("Relational operators require operands of the same type");
                    ctx->error_count++;
                    return SYMBOL_UNKNOWN;
                }
                return SYMBOL_BOOLEAN;
            }
            
            // Operadores lógicos requerem boolean
            if (strcmp(op, "and") == 0 || strcmp(op, "or") == 0) {
                if (left_type != SYMBOL_BOOLEAN || right_type != SYMBOL_BOOLEAN) {
                    semantic_error("Logical operators require boolean operands");
                    ctx->error_count++;
                    return SYMBOL_UNKNOWN;
                }
                return SYMBOL_BOOLEAN;
            }
            
            return SYMBOL_UNKNOWN;
        }
            
        case AST_UNARY_OP: {
            SymbolType operand_type = semantic_check_expression(expr->data.unary_op.operand, ctx);
            const char *op = expr->data.unary_op.operator;
            
            if (strcmp(op, "not") == 0) {
                if (operand_type != SYMBOL_BOOLEAN) {
                    semantic_error("NOT operator requires boolean operand");
                    ctx->error_count++;
                    return SYMBOL_UNKNOWN;
                }
                return SYMBOL_BOOLEAN;
            }
            
            if (strcmp(op, "-") == 0) {
                if (operand_type != SYMBOL_INTEGER) {
                    semantic_error("Unary minus requires integer operand");
                    ctx->error_count++;
                    return SYMBOL_UNKNOWN;
                }
                return SYMBOL_INTEGER;
            }
            
            return SYMBOL_UNKNOWN;
        }
            
        default:
            return SYMBOL_UNKNOWN;
    }
}

void semantic_check_statement(ASTNode *node, SemanticContext *ctx) {
    if (!node) return;
    
    switch (node->type) {
        case AST_BLOCK: {
            // Processar cada statement no bloco
            for (int i = 0; i < node->data.block.count; i++) {
                semantic_check_statement(node->data.block.statements[i], ctx);
            }
            break;
        }
            
        case AST_ASSIGNMENT: {
            // Verificar se a variável foi declarada (ou declarar implicitamente)
            const char *var_name = node->data.assignment.identifier;
            Symbol *symbol = symbol_table_lookup(ctx->current_scope, var_name);
            
            // Inferir tipo da expressão
            SymbolType expr_type = semantic_check_expression(node->data.assignment.expression, ctx);
            
            if (!symbol) {
                // Declaração implícita - usar tipo da expressão, ou INTEGER se desconhecido
                SymbolType var_type = (expr_type != SYMBOL_UNKNOWN) ? expr_type : SYMBOL_INTEGER;
                symbol_table_insert(ctx->current_scope, var_name, var_type);
            } else {
                // Verificar compatibilidade de tipos
                if (symbol->type != expr_type && expr_type != SYMBOL_UNKNOWN) {
                    semantic_error("Type mismatch in assignment to '%s': expected %s, got %s",
                                 var_name, 
                                 symbol_type_to_string(symbol->type),
                                 symbol_type_to_string(expr_type));
                    ctx->error_count++;
                }
            }
            break;
        }
            
        case AST_IF_STATEMENT: {
            // Verificar condição
            SymbolType cond_type = semantic_check_expression(node->data.if_stmt.condition, ctx);
            if (cond_type != SYMBOL_BOOLEAN && cond_type != SYMBOL_UNKNOWN) {
                semantic_error("If condition must be boolean");
                ctx->error_count++;
            }
            
            // Verificar blocos then e else
            semantic_check_statement(node->data.if_stmt.then_block, ctx);
            if (node->data.if_stmt.else_block) {
                semantic_check_statement(node->data.if_stmt.else_block, ctx);
            }
            break;
        }
            
        case AST_WHILE_STATEMENT: {
            // Verificar condição
            SymbolType cond_type = semantic_check_expression(node->data.while_stmt.condition, ctx);
            if (cond_type != SYMBOL_BOOLEAN && cond_type != SYMBOL_UNKNOWN) {
                semantic_error("While condition must be boolean");
                ctx->error_count++;
            }
            
            // Verificar corpo
            semantic_check_statement(node->data.while_stmt.body, ctx);
            break;
        }
            
        case AST_PUT_LINE: {
            // Put_Line pode receber inteiro ou string
            semantic_check_expression(node->data.put_line.expression, ctx);
            break;
        }
            
        case AST_GET_LINE: {
            // Verificar se a variável existe (ou criar)
            const char *var_name = node->data.get_line.identifier;
            Symbol *symbol = symbol_table_lookup(ctx->current_scope, var_name);
            
            if (!symbol) {
                // Declaração implícita como integer
                symbol_table_insert(ctx->current_scope, var_name, SYMBOL_INTEGER);
            }
            break;
        }
            
        default:
            break;
    }
}

void semantic_analyze(ASTNode *ast, SemanticContext *ctx) {
    if (!ast) {
        semantic_error("No AST to analyze");
        return;
    }
    
    switch (ast->type) {
        case AST_PROGRAM:
            semantic_analyze(ast->data.program.procedure, ctx);
            break;
            
        case AST_PROCEDURE:
            // Adicionar procedure na tabela de símbolos
            symbol_table_insert(ctx->current_scope, ast->data.procedure.name, SYMBOL_PROCEDURE);
            
            // Entrar em novo escopo para o procedimento
            ctx->current_scope = symbol_table_enter_scope(ctx->current_scope);
            
            // Analisar o bloco
            semantic_check_statement(ast->data.procedure.block, ctx);
            
            // NÃO sair do escopo - manter para geração de código
            // ctx->current_scope = symbol_table_exit_scope(ctx->current_scope);
            break;
            
        default:
            semantic_check_statement(ast, ctx);
            break;
    }
    
    if (ctx->error_count > 0) {
        fprintf(stderr, "\nSemantic analysis completed with %d error(s)\n", ctx->error_count);
    } else {
        printf("Semantic analysis completed successfully\n");
    }
}
