#ifndef REGISTER_ALLOC_H
#define REGISTER_ALLOC_H

#include <stdlib.h>
#include <string.h>

// Alocador simples de registradores MIPS
typedef struct {
    int available[10];  // $t0-$t9 disponibilidade
    int stack_offset;   // Offset atual na pilha para spill
} RegisterAllocator;

// Protótipos das funções
RegisterAllocator* reg_alloc_create();
void reg_alloc_free(RegisterAllocator *alloc);
const char* reg_alloc_acquire(RegisterAllocator *alloc);
void reg_alloc_release(RegisterAllocator *alloc, const char *reg);
int reg_alloc_get_stack_offset(RegisterAllocator *alloc);

#endif
