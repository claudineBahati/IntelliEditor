#include "memory_pool.h"
#include <stdlib.h>
#include <string.h>

MemoryPool* pool_create(size_t block_size, size_t num_blocks) {
    if (block_size < sizeof(void*)) block_size = sizeof(void*);
    
    MemoryPool* pool = (MemoryPool*)malloc(sizeof(MemoryPool));
    if (!pool) return NULL;
    
    pool->block_size = block_size;
    pool->num_blocks = num_blocks;
    pool->used_count = 0;
    
    pool->buffer = malloc(block_size * num_blocks);
    if (!pool->buffer) {
        free(pool);
        return NULL;
    }
    
    // Initialiser la liste chaînée des blocs libres
    pool->next_free = pool->buffer;
    char* curr = (char*)pool->buffer;
    for (size_t i = 0; i < num_blocks - 1; i++) {
        *(void**)curr = (void*)(curr + block_size);
        curr += block_size;
    }
    *(void**)curr = NULL; // Dernier bloc
    
    return pool;
}

void* pool_alloc(MemoryPool* pool) {
    if (!pool || !pool->next_free) return NULL;
    
    void* ptr = pool->next_free;
    pool->next_free = *(void**)ptr;
    pool->used_count++;
    
    return ptr;
}

void pool_free(MemoryPool* pool, void* ptr) {
    if (!pool || !ptr) return;
    
    // Remettre le bloc dans la liste des libres
    *(void**)ptr = pool->next_free;
    pool->next_free = ptr;
    pool->used_count--;
}

void pool_destroy(MemoryPool* pool) {
    if (pool) {
        if (pool->buffer) free(pool->buffer);
        free(pool);
    }
}
