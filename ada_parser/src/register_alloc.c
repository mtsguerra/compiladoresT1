#define _GNU_SOURCE
#include "register_alloc.h"
#include <stdio.h>

RegisterAllocator* reg_alloc_create() {
    RegisterAllocator *alloc = (RegisterAllocator*)malloc(sizeof(RegisterAllocator));
    
    // Inicializar todos os registradores como disponíveis
    for (int i = 0; i < 10; i++) {
        alloc->available[i] = 1;
    }
    
    alloc->stack_offset = 0;
    return alloc;
}

void reg_alloc_free(RegisterAllocator *alloc) {
    if (alloc) {
        free(alloc);
    }
}

const char* reg_alloc_acquire(RegisterAllocator *alloc) {
    // Procurar primeiro registrador disponível
    for (int i = 0; i < 10; i++) {
        if (alloc->available[i]) {
            alloc->available[i] = 0;
            
            // Retornar nome do registrador
            static char reg_names[10][4] = {
                "$t0", "$t1", "$t2", "$t3", "$t4",
                "$t5", "$t6", "$t7", "$t8", "$t9"
            };
            return reg_names[i];
        }
    }
    
    // Se não houver registrador disponível, retornar NULL
    // (em uma implementação completa, faria spill para a pilha)
    return NULL;
}

void reg_alloc_release(RegisterAllocator *alloc, const char *reg) {
    // Mapear nome do registrador para índice
    if (reg && reg[0] == '$' && reg[1] == 't') {
        int index = reg[2] - '0';
        if (index >= 0 && index <= 9) {
            alloc->available[index] = 1;
        }
    }
}

int reg_alloc_get_stack_offset(RegisterAllocator *alloc) {
    int offset = alloc->stack_offset;
    alloc->stack_offset += 4;
    return offset;
}
