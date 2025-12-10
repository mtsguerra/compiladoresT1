#ifndef MIPS_CODEGEN_H
#define MIPS_CODEGEN_H

#include <stdio.h>
#include "ast.h"
#include "symbol_table.h"
#include "register_alloc.h"

// Gerador de código MIPS
typedef struct {
    FILE *output;
    SymbolTable *symbol_table;
    RegisterAllocator *reg_alloc;
    int label_counter;
    int string_counter;
} MIPSCodeGen;

// Protótipos das funções
MIPSCodeGen* mips_codegen_create(FILE *output, SymbolTable *table);
void mips_codegen_free(MIPSCodeGen *gen);
void mips_codegen_generate(MIPSCodeGen *gen, ASTNode *ast);

// Seções do MIPS
void mips_emit_data_section(MIPSCodeGen *gen, ASTNode *ast);
void mips_emit_text_section(MIPSCodeGen *gen, ASTNode *ast);

// Geração de statements
void mips_gen_statement(MIPSCodeGen *gen, ASTNode *stmt);
void mips_gen_assignment(MIPSCodeGen *gen, ASTNode *node);
void mips_gen_if(MIPSCodeGen *gen, ASTNode *node);
void mips_gen_while(MIPSCodeGen *gen, ASTNode *node);
void mips_gen_put_line(MIPSCodeGen *gen, ASTNode *node);
void mips_gen_get_line(MIPSCodeGen *gen, ASTNode *node);

// Geração de expressões (retorna registrador usado)
const char* mips_gen_expression(MIPSCodeGen *gen, ASTNode *expr);
const char* mips_gen_binary_op(MIPSCodeGen *gen, ASTNode *node);

// Helpers
const char* mips_new_label(MIPSCodeGen *gen);
const char* mips_new_string_label(MIPSCodeGen *gen);
void mips_emit(MIPSCodeGen *gen, const char *format, ...);

#endif
