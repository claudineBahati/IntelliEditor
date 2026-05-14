#define DEBUG_MEMORY_IMPLEMENTATION
#include "debug_memory.h"
#include "memory_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t total_allocated = 0;
static size_t allocation_count = 0;
static size_t pool_alloc_count = 0;

// Pools pour les petits objets
static MemoryPool* pool32 = NULL;
static MemoryPool* pool64 = NULL;
static MemoryPool* pool128 = NULL;
static bool pools_initialized = false;

// Indicateur pour savoir si un bloc vient d'un pool
#define POOL_MARKER (size_t)0xDEADBEEFCAFEBABEL

static void init_pools(void) {
    if (pools_initialized) return;
    pool32 = pool_create(32 + sizeof(size_t) * 2, 1024);
    pool64 = pool_create(64 + sizeof(size_t) * 2, 512);
    pool128 = pool_create(128 + sizeof(size_t) * 2, 256);
    pools_initialized = true;
}

void* debug_malloc(size_t size, const char* file, int line) {
    if (!pools_initialized) {
        init_pools();
    }
    
    void* ptr = NULL;
    size_t actual_size = size + sizeof(size_t) * 2;
    size_t pool_id = 0;

    if (size <= 32) {
        ptr = pool_alloc(pool32);
        pool_id = 32;
    } else if (size <= 64) {
        ptr = pool_alloc(pool64);
        pool_id = 64;
    } else if (size <= 128) {
        ptr = pool_alloc(pool128);
        pool_id = 128;
    }

    if (ptr) {
        pool_alloc_count++;
    } else {
        ptr = malloc(actual_size);
        pool_id = 0;
    }

    if (!ptr) return NULL;
    
    // Stockage des métadonnées : [Marker][Size][Données...]
    ((size_t*)ptr)[0] = (pool_id == 0) ? 0 : POOL_MARKER;
    ((size_t*)ptr)[1] = size;
    
    total_allocated += size;
    allocation_count++;
    
    return (void*)((char*)ptr + sizeof(size_t) * 2);
}

void* debug_realloc(void* ptr, size_t size, const char* file, int line) {
    if (!ptr) return debug_malloc(size, file, line);
    
    void* real_ptr = (void*)((char*)ptr - sizeof(size_t) * 2);
    size_t old_size = ((size_t*)real_ptr)[1];
    
    // Pour realloc, on passe toujours par la heap pour simplifier
    // (on pourrait optimiser si le nouveau size rentre dans le même pool)
    void* new_ptr = debug_malloc(size, file, line);
    if (!new_ptr) return NULL;
    
    memcpy(new_ptr, ptr, (size < old_size) ? size : old_size);
    debug_free(ptr);
    
    return new_ptr;
}

void* debug_calloc(size_t nmemb, size_t size, const char* file, int line) {
    size_t total = nmemb * size;
    void* ptr = debug_malloc(total, file, line);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

char* debug_strdup(const char* s, const char* file, int line) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* ptr = (char*)debug_malloc(len, file, line);
    if (ptr) memcpy(ptr, s, len);
    return ptr;
}

void debug_free(void* ptr) {
    if (!ptr) return;
    
    void* real_ptr = (void*)((char*)ptr - sizeof(size_t) * 2);
    size_t marker = ((size_t*)real_ptr)[0];
    size_t size = ((size_t*)real_ptr)[1];
    
    total_allocated -= size;
    allocation_count--;
    
    if (marker == POOL_MARKER) {
        // Déterminer quel pool a été utilisé
        if (size <= 32) pool_free(pool32, real_ptr);
        else if (size <= 64) pool_free(pool64, real_ptr);
        else if (size <= 128) pool_free(pool128, real_ptr);
    } else {
        free(real_ptr);
    }
}

void debug_memory_report(void) {
    printf("\n--- RAPPORT MEMOIRE (Optimisé) ---\n");
    printf("Total alloué : %zu octets\n", total_allocated);
    printf("Allocations actives : %zu\n", allocation_count);
    printf("Allocations via Pools : %zu\n", pool_alloc_count);
    
    if (pools_initialized) {
        printf("Utilisation Pools : 32b:%zu, 64b:%zu, 128b:%zu\n", 
               pool32->used_count, pool64->used_count, pool128->used_count);
    }

    if (allocation_count > 0) {
        printf("ATTENTION : %zu fuites de mémoire détectées !\n", allocation_count);
    } else {
        printf("Aucune fuite de mémoire détectée. Félicitations !\n");
    }
    printf("------------------------\n\n");
    fflush(stdout);

    // On détruit les pools à la fin du rapport (fin du programme)
    if (pools_initialized) {
        pool_destroy(pool32);
        pool_destroy(pool64);
        pool_destroy(pool128);
        pools_initialized = false;
    }
}
