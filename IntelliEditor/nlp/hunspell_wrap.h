#ifndef HUNSPELL_WRAP_H
#define HUNSPELL_WRAP_H

#include <stdbool.h>

// Initialise le correcteur
bool nlp_init(const char* aff_path, const char* dic_path);

// Vérifie un mot
bool nlp_check_word(const char* word);

// Libère la mémoire
void nlp_cleanup();

#endif