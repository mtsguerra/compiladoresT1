#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>

// Tipos de símbolos suportados
typedef enum {
    SYMBOL_INTEGER,
    SYMBOL_STRING,
    SYMBOL_BOOLEAN,
    SYMBOL_PROCEDURE,
    SYMBOL_FUNCTION,
    SYMBOL_UNKNOWN
} SymbolType;

// Representa um símbolo na tabela
typedef struct Symbol {
    char *name;
    SymbolType type;
    int offset;  // Offset na memória (para variáveis locais)
    struct Symbol *next;  // Para lista encadeada
} Symbol;

// Tabela de símbolos com suporte a escopos aninhados
typedef struct SymbolTable {
    Symbol *symbols;  // Lista encadeada de símbolos
    struct SymbolTable *parent;  // Escopo pai
    int scope_level;
    int next_offset;  // Próximo offset disponível para variáveis
} SymbolTable;

// Protótipos das funções
SymbolTable* symbol_table_create(SymbolTable *parent);
void symbol_table_free(SymbolTable *table);
void symbol_table_insert(SymbolTable *table, const char *name, SymbolType type);
Symbol* symbol_table_lookup(SymbolTable *table, const char *name);
Symbol* symbol_table_lookup_local(SymbolTable *table, const char *name);
SymbolTable* symbol_table_enter_scope(SymbolTable *current);
SymbolTable* symbol_table_exit_scope(SymbolTable *current);
const char* symbol_type_to_string(SymbolType type);

#endif
