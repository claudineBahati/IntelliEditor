#ifndef DEBUG_MEMORY_H
#define DEBUG_MEMORY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * Alloue de la mémoire et incrémente le compteur de suivi.
 */
void* debug_malloc(size_t size, const char* file, int line);
void* debug_realloc(void* ptr, size_t size, const char* file, int line);
void* debug_calloc(size_t nmemb, size_t size, const char* file, int line);
char* debug_strdup(const char* s, const char* file, int line);
void debug_free(void* ptr);
void debug_memory_report(void);

// Ces macros permettent de capturer les appels sans modifier tout le code source.
// Elles ne sont actives que si DEBUG_MEMORY_IMPLEMENTATION n'est PAS défini.
#ifndef DEBUG_MEMORY_IMPLEMENTATION
    #define malloc(s) debug_malloc(s, __FILE__, __LINE__)
    #define realloc(p, s) debug_realloc(p, s, __FILE__, __LINE__)
    #define calloc(n, s) debug_calloc(n, s, __FILE__, __LINE__)
    #define strdup(s) debug_strdup(s, __FILE__, __LINE__)
    #define free(p) debug_free(p)
#endif

#endif // DEBUG_MEMORY_H
