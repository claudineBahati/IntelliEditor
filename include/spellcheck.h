#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_SUGGESTIONS 5
#define MAX_WORD_LENGTH 64

// Structure pour stocker les suggestions de correction
typedef struct {
    char words[MAX_SUGGESTIONS][MAX_WORD_LENGTH];
    int count;
} SpellSuggestions;

// Initialise le correcteur orthographique (charge le dictionnaire)
// filepath: chemin vers le fichier texte contenant un mot par ligne
bool spellcheck_init(const char* dict_filepath);

// Libère la mémoire allouée par le correcteur
void spellcheck_cleanup(void);

// Vérifie si un mot est correctement orthographié (existe dans le dictionnaire)
bool spellcheck_is_correct(const char* word);

// Propose des corrections pour un mot mal orthographié
SpellSuggestions spellcheck_get_suggestions(const char* misspelled_word);

// Calcule la distance de Levenshtein entre deux mots (plus le score est bas, plus ils se ressemblent)
int levenshtein_distance(const char* s1, const char* s2);

#endif // SPELLCHECK_H
