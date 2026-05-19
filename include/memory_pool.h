#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Un pool de mémoire pour des allocations de taille fixe.
 * Très performant pour les structures répétitives (StyleRange, EditorAction, etc.).
 */
typedef struct {
    void* buffer;
    size_t block_size;
    size_t num_blocks;
    void* next_free;
    size_t used_count;
} MemoryPool;

/**
 * Crée un nouveau pool.
 */
MemoryPool* pool_create(size_t block_size, size_t num_blocks);

/**
 * Alloue un bloc depuis le pool.
 */
void* pool_alloc(MemoryPool* pool);

/**
 * Libère un bloc dans le pool.
 */
void pool_free(MemoryPool* pool, void* ptr);

/**
 * Détruit le pool.
 */
void pool_destroy(MemoryPool* pool);

#endif // MEMORY_POOL_H
