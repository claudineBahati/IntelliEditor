#ifndef HUNSPELL_WRAP_H
#define HUNSPELL_WRAP_H

#include <stdbool.h>

// Initialise le correcteur Hunspell
bool hunspell_wrap_init(const char* aff_path, const char* dic_path);

// Vérifie un mot
bool hunspell_wrap_check_word(const char* word);

// Libère la mémoire
void hunspell_wrap_cleanup();

#endif