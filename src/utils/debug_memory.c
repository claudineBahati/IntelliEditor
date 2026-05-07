#define DEBUG_MEMORY_IMPLEMENTATION
#include "debug_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* On n'a plus besoin des #undef car les macros sont protégées dans le header */

static size_t total_allocated = 0;
static size_t allocation_count = 0;

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size + sizeof(size_t));
    if (!ptr) return NULL;
    
    *(size_t*)ptr = size;
    total_allocated += size;
    allocation_count++;
    
    return (void*)((char*)ptr + sizeof(size_t));
}

void* debug_realloc(void* ptr, size_t size, const char* file, int line) {
    if (!ptr) return debug_malloc(size, file, line);
    
    void* real_ptr = (void*)((char*)ptr - sizeof(size_t));
    size_t old_size = *(size_t*)real_ptr;
    
    void* new_ptr = realloc(real_ptr, size + sizeof(size_t));
    if (!new_ptr) return NULL;
    
    *(size_t*)new_ptr = size;
    total_allocated = total_allocated - old_size + size;
    
    return (void*)((char*)new_ptr + sizeof(size_t));
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
    
    void* real_ptr = (void*)((char*)ptr - sizeof(size_t));
    size_t size = *(size_t*)real_ptr;
    
    total_allocated -= size;
    allocation_count--;
    
    free(real_ptr);
}

void debug_memory_report(void) {
    printf("\n--- RAPPORT MEMOIRE ---\n");
    printf("Total alloué : %zu octets\n", total_allocated);
    printf("Allocations actives : %zu\n", allocation_count);
    if (allocation_count > 0) {
        printf("ATTENTION : %zu fuites de mémoire détectées !\n", allocation_count);
    } else {
        printf("Aucune fuite de mémoire détectée. Félicitations !\n");
    }
    printf("------------------------\n\n");
    fflush(stdout);
}
