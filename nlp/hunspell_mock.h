#ifndef HUNSPELL_MOCK_H
#define HUNSPELL_MOCK_H

#include <stdio.h>
#include <string.h>

// Mock structure and functions for Hunspell
typedef struct {
    int dummy;
} Hunhandle;

static inline Hunhandle* Hunspell_create(const char* aff, const char* dic) {
    printf("[MOCK] Hunspell_create called with %s and %s\n", aff, dic);
    return (Hunhandle*)1; // Return dummy pointer
}

static inline void Hunspell_destroy(Hunhandle* h) {
    printf("[MOCK] Hunspell_destroy called\n");
}

static inline int Hunspell_spell(Hunhandle* h, const char* word) {
    // Return 1 (correct) for everything for now, or 0 for specific words to test
    if (word && (strcmp(word, "error") == 0 || strcmp(word, "mistake") == 0)) return 0;
    return 1; 
}

static inline int Hunspell_suggest(Hunhandle* h, char*** slist, const char* word) {
    *slist = NULL;
    return 0;
}

static inline void Hunspell_free_list(Hunhandle* h, char*** slist, int n) {
}

#endif // HUNSPELL_MOCK_H
