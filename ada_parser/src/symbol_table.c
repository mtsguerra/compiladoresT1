#define _GNU_SOURCE
#include "symbol_table.h"
#include <stdio.h>

SymbolTable* symbol_table_create(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->symbols = NULL;
    table->parent = parent;
    table->scope_level = parent ? parent->scope_level + 1 : 0;
    table->next_offset = 0;
    return table;
}

void symbol_table_free(SymbolTable *table) {
    if (!table) return;
    
    // Liberar todos os símbolos
    Symbol *current = table->symbols;
    while (current) {
        Symbol *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    
    free(table);
}

void symbol_table_insert(SymbolTable *table, const char *name, SymbolType type) {
    // Verificar se já existe no escopo local
    Symbol *existing = symbol_table_lookup_local(table, name);
    if (existing) {
        fprintf(stderr, "Semantic error: Variable '%s' already declared in current scope\n", name);
        return;
    }
    
    // Criar novo símbolo
    Symbol *symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->type = type;
    symbol->offset = table->next_offset;
    table->next_offset += 4;  // Assume 4 bytes por variável
    
    // Adicionar no início da lista
    symbol->next = table->symbols;
    table->symbols = symbol;
}

Symbol* symbol_table_lookup(SymbolTable *table, const char *name) {
    // Buscar no escopo atual
    Symbol *symbol = symbol_table_lookup_local(table, name);
    if (symbol) {
        return symbol;
    }
    
    // Buscar recursivamente nos escopos pais
    if (table->parent) {
        return symbol_table_lookup(table->parent, name);
    }
    
    return NULL;
}

Symbol* symbol_table_lookup_local(SymbolTable *table, const char *name) {
    Symbol *current = table->symbols;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

SymbolTable* symbol_table_enter_scope(SymbolTable *current) {
    return symbol_table_create(current);
}

SymbolTable* symbol_table_exit_scope(SymbolTable *current) {
    if (!current || !current->parent) {
        return current;
    }
    
    SymbolTable *parent = current->parent;
    symbol_table_free(current);
    return parent;
}

const char* symbol_type_to_string(SymbolType type) {
    switch (type) {
        case SYMBOL_INTEGER: return "Integer";
        case SYMBOL_STRING: return "String";
        case SYMBOL_BOOLEAN: return "Boolean";
        case SYMBOL_PROCEDURE: return "Procedure";
        case SYMBOL_FUNCTION: return "Function";
        case SYMBOL_UNKNOWN: return "Unknown";
        default: return "Invalid";
    }
}
