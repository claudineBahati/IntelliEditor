#ifndef SEARCH_H
#define SEARCH_H

#include "gap_buffer.h"
#include <stdbool.h>

/**
 * Recherche une chaîne de caractères dans le Gap Buffer.
 * @param gb Le buffer dans lequel chercher.
 * @param query La chaîne à rechercher.
 * @param start_pos L'index de départ de la recherche.
 * @param case_sensitive True pour une recherche sensible à la casse.
 * @return L'index du début de la première occurrence trouvée, ou (size_t)-1 si non trouvé.
 */
size_t search_find(const GapBuffer* gb, const char* query, size_t start_pos, bool case_sensitive);

#endif // SEARCH_H
