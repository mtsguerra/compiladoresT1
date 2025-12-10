#define _GNU_SOURCE
#include "mips_codegen.h"
#include <stdarg.h>
#include <string.h>

MIPSCodeGen* mips_codegen_create(FILE *output, SymbolTable *table) {
    MIPSCodeGen *gen = (MIPSCodeGen*)malloc(sizeof(MIPSCodeGen));
    gen->output = output;
    gen->symbol_table = table;
    gen->reg_alloc = reg_alloc_create();
    gen->label_counter = 0;
    gen->string_counter = 0;
    return gen;
}

void mips_codegen_free(MIPSCodeGen *gen) {
    if (!gen) return;
    if (gen->reg_alloc) {
        reg_alloc_free(gen->reg_alloc);
    }
    free(gen);
}

void mips_emit(MIPSCodeGen *gen, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(gen->output, format, args);
    va_end(args);
}

const char* mips_new_label(MIPSCodeGen *gen) {
    // Use multiple static buffers to avoid overwriting
    static char labels[100][32];
    static int buffer_index = 0;
    
    int current = buffer_index;
    buffer_index = (buffer_index + 1) % 100;
    
    snprintf(labels[current], sizeof(labels[current]), "L%d", gen->label_counter++);
    return labels[current];
}

const char* mips_new_string_label(MIPSCodeGen *gen) {
    static char labels[100][32];
    static int buffer_index = 0;
    
    int current = buffer_index;
    buffer_index = (buffer_index + 1) % 100;
    
    snprintf(labels[current], sizeof(labels[current]), "str%d", gen->string_counter++);
    return labels[current];
}

// Coleta todas as strings literais do AST
void collect_strings(ASTNode *node, MIPSCodeGen *gen) {
    if (!node) return;
    
    switch (node->type) {
        case AST_STRING: {
            const char *label = mips_new_string_label(gen);
            mips_emit(gen, "%s: .asciiz \"%s\"\n", label, node->data.string.value);
            break;
        }
        case AST_PROGRAM:
            collect_strings(node->data.program.procedure, gen);
            break;
        case AST_PROCEDURE:
            collect_strings(node->data.procedure.block, gen);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->data.block.count; i++) {
                collect_strings(node->data.block.statements[i], gen);
            }
            break;
        case AST_ASSIGNMENT:
            collect_strings(node->data.assignment.expression, gen);
            break;
        case AST_IF_STATEMENT:
            collect_strings(node->data.if_stmt.condition, gen);
            collect_strings(node->data.if_stmt.then_block, gen);
            collect_strings(node->data.if_stmt.else_block, gen);
            break;
        case AST_WHILE_STATEMENT:
            collect_strings(node->data.while_stmt.condition, gen);
            collect_strings(node->data.while_stmt.body, gen);
            break;
        case AST_PUT_LINE:
            collect_strings(node->data.put_line.expression, gen);
            break;
        case AST_BINARY_OP:
            collect_strings(node->data.binary_op.left, gen);
            collect_strings(node->data.binary_op.right, gen);
            break;
        case AST_UNARY_OP:
            collect_strings(node->data.unary_op.operand, gen);
            break;
        default:
            break;
    }
}

void mips_emit_data_section(MIPSCodeGen *gen, ASTNode *ast) {
    mips_emit(gen, ".data\n");
    
    // Coletar e emitir strings literais
    gen->string_counter = 0;  // Reset counter
    collect_strings(ast, gen);
    
    // Adicionar newline para Put_Line
    mips_emit(gen, "newline: .asciiz \"\\n\"\n");
    
    mips_emit(gen, "\n");
}

const char* mips_gen_expression(MIPSCodeGen *gen, ASTNode *expr) {
    if (!expr) return NULL;
    
    const char *reg = NULL;
    
    switch (expr->type) {
        case AST_INTEGER: {
            reg = reg_alloc_acquire(gen->reg_alloc);
            if (!reg) {
                fprintf(stderr, "Error: No available registers\n");
                return NULL;
            }
            mips_emit(gen, "    li %s, %d\n", reg, expr->data.integer.value);
            return reg;
        }
            
        case AST_IDENTIFIER: {
            // Carregar valor da variável
            Symbol *symbol = symbol_table_lookup(gen->symbol_table, expr->data.identifier.name);
            if (!symbol) {
                fprintf(stderr, "Error: Undefined variable '%s'\n", expr->data.identifier.name);
                return NULL;
            }
            
            reg = reg_alloc_acquire(gen->reg_alloc);
            if (!reg) {
                fprintf(stderr, "Error: No available registers\n");
                return NULL;
            }
            
            // Carregar do offset relativo ao $fp
            mips_emit(gen, "    lw %s, %d($fp)\n", reg, symbol->offset);
            return reg;
        }
            
        case AST_BINARY_OP:
            return mips_gen_binary_op(gen, expr);
            
        case AST_STRING: {
            // Para strings, precisamos retornar o endereço
            reg = reg_alloc_acquire(gen->reg_alloc);
            if (!reg) {
                fprintf(stderr, "Error: No available registers\n");
                return NULL;
            }
            
            // Precisamos mapear a string para seu label
            // Por simplicidade, vamos usar um contador global
            static int str_index = 0;
            mips_emit(gen, "    la %s, str%d\n", reg, str_index++);
            return reg;
        }
            
        case AST_UNARY_OP: {
            const char *operand_reg = mips_gen_expression(gen, expr->data.unary_op.operand);
            if (!operand_reg) return NULL;
            
            if (strcmp(expr->data.unary_op.operator, "-") == 0) {
                mips_emit(gen, "    neg %s, %s\n", operand_reg, operand_reg);
            } else if (strcmp(expr->data.unary_op.operator, "not") == 0) {
                mips_emit(gen, "    xori %s, %s, 1\n", operand_reg, operand_reg);
            }
            
            return operand_reg;
        }
            
        default:
            return NULL;
    }
}

const char* mips_gen_binary_op(MIPSCodeGen *gen, ASTNode *node) {
    const char *left_reg = mips_gen_expression(gen, node->data.binary_op.left);
    const char *right_reg = mips_gen_expression(gen, node->data.binary_op.right);
    
    if (!left_reg || !right_reg) return NULL;
    
    const char *op = node->data.binary_op.operator;
    const char *result_reg = left_reg;  // Reusar registrador da esquerda
    
    // Operadores aritméticos
    if (strcmp(op, "+") == 0) {
        mips_emit(gen, "    add %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "-") == 0) {
        mips_emit(gen, "    sub %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "*") == 0) {
        mips_emit(gen, "    mul %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "/") == 0) {
        mips_emit(gen, "    div %s, %s, %s\n", result_reg, left_reg, right_reg);
    }
    // Operadores relacionais
    else if (strcmp(op, "<") == 0) {
        mips_emit(gen, "    slt %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, ">") == 0) {
        mips_emit(gen, "    sgt %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "<=") == 0) {
        mips_emit(gen, "    sle %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, ">=") == 0) {
        mips_emit(gen, "    sge %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "=") == 0) {
        mips_emit(gen, "    seq %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "/=") == 0) {
        mips_emit(gen, "    sne %s, %s, %s\n", result_reg, left_reg, right_reg);
    }
    // Operadores lógicos
    else if (strcmp(op, "and") == 0) {
        mips_emit(gen, "    and %s, %s, %s\n", result_reg, left_reg, right_reg);
    } else if (strcmp(op, "or") == 0) {
        mips_emit(gen, "    or %s, %s, %s\n", result_reg, left_reg, right_reg);
    }
    
    // Liberar registrador da direita
    reg_alloc_release(gen->reg_alloc, right_reg);
    
    return result_reg;
}

void mips_gen_assignment(MIPSCodeGen *gen, ASTNode *node) {
    const char *expr_reg = mips_gen_expression(gen, node->data.assignment.expression);
    if (!expr_reg) return;
    
    // Obter símbolo
    Symbol *symbol = symbol_table_lookup(gen->symbol_table, node->data.assignment.identifier);
    if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data.assignment.identifier);
        reg_alloc_release(gen->reg_alloc, expr_reg);
        return;
    }
    
    // Armazenar no offset relativo ao $fp
    mips_emit(gen, "    sw %s, %d($fp)\n", expr_reg, symbol->offset);
    
    // Liberar registrador
    reg_alloc_release(gen->reg_alloc, expr_reg);
}

void mips_gen_if(MIPSCodeGen *gen, ASTNode *node) {
    const char *else_label = mips_new_label(gen);
    const char *end_label = mips_new_label(gen);
    
    // Avaliar condição
    const char *cond_reg = mips_gen_expression(gen, node->data.if_stmt.condition);
    if (!cond_reg) return;
    
    // Se condição é falsa, pular para else
    mips_emit(gen, "    beqz %s, %s\n", cond_reg, else_label);
    reg_alloc_release(gen->reg_alloc, cond_reg);
    
    // Bloco then
    mips_gen_statement(gen, node->data.if_stmt.then_block);
    
    // Pular para o fim
    mips_emit(gen, "    j %s\n", end_label);
    
    // Bloco else
    mips_emit(gen, "%s:\n", else_label);
    if (node->data.if_stmt.else_block) {
        mips_gen_statement(gen, node->data.if_stmt.else_block);
    }
    
    // Label de fim
    mips_emit(gen, "%s:\n", end_label);
}

void mips_gen_while(MIPSCodeGen *gen, ASTNode *node) {
    const char *start_label = mips_new_label(gen);
    const char *end_label = mips_new_label(gen);
    
    // Label de início do loop
    mips_emit(gen, "%s:\n", start_label);
    
    // Avaliar condição
    const char *cond_reg = mips_gen_expression(gen, node->data.while_stmt.condition);
    if (!cond_reg) return;
    
    // Se condição é falsa, sair do loop
    mips_emit(gen, "    beqz %s, %s\n", cond_reg, end_label);
    reg_alloc_release(gen->reg_alloc, cond_reg);
    
    // Corpo do loop
    mips_gen_statement(gen, node->data.while_stmt.body);
    
    // Voltar ao início
    mips_emit(gen, "    j %s\n", start_label);
    
    // Label de fim
    mips_emit(gen, "%s:\n", end_label);
}

void mips_gen_put_line(MIPSCodeGen *gen, ASTNode *node) {
    ASTNode *expr = node->data.put_line.expression;
    
    if (expr->type == AST_INTEGER || expr->type == AST_IDENTIFIER || 
        expr->type == AST_BINARY_OP) {
        // Imprimir inteiro
        const char *reg = mips_gen_expression(gen, expr);
        if (!reg) return;
        
        mips_emit(gen, "    move $a0, %s\n", reg);
        mips_emit(gen, "    li $v0, 1\n");  // syscall print_int
        mips_emit(gen, "    syscall\n");
        
        reg_alloc_release(gen->reg_alloc, reg);
        
        // Imprimir newline
        mips_emit(gen, "    la $a0, newline\n");
        mips_emit(gen, "    li $v0, 4\n");  // syscall print_string
        mips_emit(gen, "    syscall\n");
    } else if (expr->type == AST_STRING) {
        // Imprimir string
        const char *reg = mips_gen_expression(gen, expr);
        if (!reg) return;
        
        mips_emit(gen, "    move $a0, %s\n", reg);
        mips_emit(gen, "    li $v0, 4\n");  // syscall print_string
        mips_emit(gen, "    syscall\n");
        
        reg_alloc_release(gen->reg_alloc, reg);
        
        // Imprimir newline
        mips_emit(gen, "    la $a0, newline\n");
        mips_emit(gen, "    li $v0, 4\n");
        mips_emit(gen, "    syscall\n");
    }
}

void mips_gen_get_line(MIPSCodeGen *gen, ASTNode *node) {
    // Obter símbolo
    Symbol *symbol = symbol_table_lookup(gen->symbol_table, node->data.get_line.identifier);
    if (!symbol) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data.get_line.identifier);
        return;
    }
    
    // Ler inteiro
    mips_emit(gen, "    li $v0, 5\n");  // syscall read_int
    mips_emit(gen, "    syscall\n");
    
    // Armazenar resultado
    mips_emit(gen, "    sw $v0, %d($fp)\n", symbol->offset);
}

void mips_gen_statement(MIPSCodeGen *gen, ASTNode *stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case AST_BLOCK:
            for (int i = 0; i < stmt->data.block.count; i++) {
                mips_gen_statement(gen, stmt->data.block.statements[i]);
            }
            break;
            
        case AST_ASSIGNMENT:
            mips_gen_assignment(gen, stmt);
            break;
            
        case AST_IF_STATEMENT:
            mips_gen_if(gen, stmt);
            break;
            
        case AST_WHILE_STATEMENT:
            mips_gen_while(gen, stmt);
            break;
            
        case AST_PUT_LINE:
            mips_gen_put_line(gen, stmt);
            break;
            
        case AST_GET_LINE:
            mips_gen_get_line(gen, stmt);
            break;
            
        default:
            break;
    }
}

void mips_emit_text_section(MIPSCodeGen *gen, ASTNode *ast) {
    mips_emit(gen, ".text\n");
    mips_emit(gen, ".globl main\n\n");
    
    if (ast->type == AST_PROGRAM) {
        ast = ast->data.program.procedure;
    }
    
    if (ast->type == AST_PROCEDURE) {
        mips_emit(gen, "main:\n");
        
        // Prólogo
        mips_emit(gen, "    # Procedure prologue\n");
        mips_emit(gen, "    addi $sp, $sp, -4\n");
        mips_emit(gen, "    sw $fp, 0($sp)\n");
        mips_emit(gen, "    move $fp, $sp\n");
        
        // Alocar espaço para variáveis locais
        // Calcular tamanho necessário baseado na tabela de símbolos
        int frame_size = gen->symbol_table->next_offset;
        if (frame_size > 0) {
            mips_emit(gen, "    addi $sp, $sp, -%d\n", frame_size);
        }
        
        mips_emit(gen, "\n");
        
        // Gerar código para o bloco
        mips_gen_statement(gen, ast->data.procedure.block);
        
        mips_emit(gen, "\n");
        
        // Epílogo
        mips_emit(gen, "    # Procedure epilogue\n");
        if (frame_size > 0) {
            mips_emit(gen, "    addi $sp, $sp, %d\n", frame_size);
        }
        mips_emit(gen, "    lw $fp, 0($sp)\n");
        mips_emit(gen, "    addi $sp, $sp, 4\n");
        
        // Encerrar programa
        mips_emit(gen, "    li $v0, 10\n");  // syscall exit
        mips_emit(gen, "    syscall\n");
    }
}

void mips_codegen_generate(MIPSCodeGen *gen, ASTNode *ast) {
    if (!gen || !ast) return;
    
    mips_emit(gen, "# Generated MIPS Assembly\n");
    mips_emit(gen, "# Ada to MIPS Compiler\n\n");
    
    mips_emit_data_section(gen, ast);
    mips_emit_text_section(gen, ast);
}
