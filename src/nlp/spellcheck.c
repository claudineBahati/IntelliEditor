#include "spellcheck.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_TABLE_SIZE 100003 // Un grand nombre premier pour minimiser les collisions

// Nœud pour notre table de hachage (gestion des collisions par chaînage)
typedef struct DictNode {
    char word[MAX_WORD_LENGTH];
    struct DictNode* next;
} DictNode;

static DictNode* dictionary[HASH_TABLE_SIZE] = {NULL};
static bool is_initialized = false;

// Fonction de hachage (djb2 par Dan Bernstein)
static unsigned long hash_word(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + tolower(c); // hash * 33 + c
    }
    return hash % HASH_TABLE_SIZE;
}

// Convertit un mot en minuscules pour la comparaison
static void to_lowercase(char* dest, const char* src) {
    int i = 0;
    while (src[i] && i < MAX_WORD_LENGTH - 1) {
        dest[i] = tolower(src[i]);
        i++;
    }
    dest[i] = '\0';
}

bool spellcheck_init(const char* dict_filepath) {
    if (is_initialized) return true;

    FILE* file = fopen(dict_filepath, "r");
    if (!file) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir le dictionnaire %s\n", dict_filepath);
        return false;
    }

    char line[MAX_WORD_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Supprimer le retour à la ligne
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
        }
        if (len > 1 && (line[len-2] == '\r')) {
            line[len-2] = '\0';
        }

        if (strlen(line) == 0) continue;

        char lower_word[MAX_WORD_LENGTH];
        to_lowercase(lower_word, line);

        unsigned long idx = hash_word(lower_word);
        
        DictNode* new_node = (DictNode*)malloc(sizeof(DictNode));
        if (new_node) {
            strncpy(new_node->word, lower_word, MAX_WORD_LENGTH - 1);
            new_node->word[MAX_WORD_LENGTH - 1] = '\0';
            new_node->next = dictionary[idx];
            dictionary[idx] = new_node;
        }
    }

    fclose(file);

    // Ajouter des contractions et particules françaises courantes pour éviter les faux positifs
    const char* french_particles[] = {
        "l", "d", "c", "s", "j", "m", "t", "n", "qu", "lorsqu", "puisqu", "quoiqu",
        "a", "à", "y", "o", "u", "g", "si", "et", "ou", "en", "un", "une", "le", "la", "les",
        "se", "ce", "dans", "par", "pour", "avec", "sans", "sous", "sur", "chez", "mais", "donc", "car"
    };
    int num_particles = sizeof(french_particles) / sizeof(french_particles[0]);
    for (int i = 0; i < num_particles; i++) {
        char lower_word[MAX_WORD_LENGTH];
        to_lowercase(lower_word, french_particles[i]);
        unsigned long idx = hash_word(lower_word);
        // Vérifier d'abord s'il n'existe pas déjà
        bool exists = false;
        DictNode* curr = dictionary[idx];
        while (curr) {
            if (strcmp(curr->word, lower_word) == 0) {
                exists = true;
                break;
            }
            curr = curr->next;
        }
        if (!exists) {
            DictNode* new_node = (DictNode*)malloc(sizeof(DictNode));
            if (new_node) {
                strncpy(new_node->word, lower_word, MAX_WORD_LENGTH - 1);
                new_node->word[MAX_WORD_LENGTH - 1] = '\0';
                new_node->next = dictionary[idx];
                dictionary[idx] = new_node;
            }
        }
    }

    is_initialized = true;
    return true;
}

void spellcheck_cleanup(void) {
    if (!is_initialized) return;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        DictNode* current = dictionary[i];
        while (current != NULL) {
            DictNode* temp = current;
            current = current->next;
            free(temp);
        }
        dictionary[i] = NULL;
    }
    is_initialized = false;
}

bool spellcheck_is_correct(const char* word) {
    if (!is_initialized) return true; // Si pas de dico, on ignore

    char lower_word[MAX_WORD_LENGTH];
    to_lowercase(lower_word, word);

    unsigned long idx = hash_word(lower_word);
    DictNode* current = dictionary[idx];

    while (current != NULL) {
        if (strcmp(current->word, lower_word) == 0) {
            return true;
        }
        current = current->next;
    }

    return false;
}

// Distance de Levenshtein (calcul dynamique classique)
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (tolower(s1[i - 1]) == tolower(s2[j - 1])) ? 0 : 1;
            
            int del = matrix[i - 1][j] + 1;
            int ins = matrix[i][j - 1] + 1;
            int sub = matrix[i - 1][j - 1] + cost;

            int min = del;
            if (ins < min) min = ins;
            if (sub < min) min = sub;

            matrix[i][j] = min;
        }
    }

    return matrix[len1][len2];
}

// Structure temporaire pour trier les suggestions
typedef struct {
    char word[MAX_WORD_LENGTH];
    int distance;
} SuggestionEntry;

SpellSuggestions spellcheck_get_suggestions(const char* misspelled_word) {
    SpellSuggestions suggestions = { .count = 0 };
    if (!is_initialized) return suggestions;

    char lower_word[MAX_WORD_LENGTH];
    to_lowercase(lower_word, misspelled_word);

    SuggestionEntry* entries = (SuggestionEntry*)malloc(HASH_TABLE_SIZE * sizeof(SuggestionEntry));
    if (!entries) return suggestions;
    int entry_count = 0;

    // On parcourt TOUT le dictionnaire pour trouver les mots les plus proches
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        DictNode* current = dictionary[i];
        while (current != NULL) {
            // Ne calculer la distance que si la longueur est proche (optimisation)
            int len_diff = abs((int)strlen(lower_word) - (int)strlen(current->word));
            if (len_diff <= 3) {
                int dist = levenshtein_distance(lower_word, current->word);
                if (dist <= 3) { // On ne garde que les mots qui ont max 3 erreurs
                    if (entry_count < HASH_TABLE_SIZE) {
                        strncpy(entries[entry_count].word, current->word, MAX_WORD_LENGTH - 1);
                        entries[entry_count].distance = dist;
                        entry_count++;
                    }
                }
            }
            current = current->next;
        }
    }

    // Tri des suggestions par distance (Bubble sort basique)
    for (int i = 0; i < entry_count - 1; i++) {
        for (int j = 0; j < entry_count - i - 1; j++) {
            if (entries[j].distance > entries[j + 1].distance) {
                SuggestionEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }

    // Récupérer les top N suggestions
    for (int i = 0; i < entry_count && suggestions.count < MAX_SUGGESTIONS; i++) {
        strncpy(suggestions.words[suggestions.count], entries[i].word, MAX_WORD_LENGTH - 1);
        suggestions.count++;
    }

    free(entries);
    return suggestions;
}
