#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>

/**
 * Structure of the Gap Buffer.
 * Le gap est le "trou" dans la mémoire où les nouvelles insertions se font.
 * Le curseur est toujours positionné à `gap_start`.
 */
typedef struct {
    char* buffer;       // Pointeur vers la mémoire allouée
    size_t size;        // Taille totale de la mémoire allouée
    size_t gap_start;   // Index où commence le gap (équivalent à la position du curseur)
    size_t gap_end;     // Index où se termine le gap
} GapBuffer;

/**
 * Crée un nouveau Gap Buffer.
 * @param initial_size La taille de départ (le buffer grandira si besoin).
 * @return Un pointeur vers la nouvelle structure, ou NULL si échec.
 */
GapBuffer* gb_create(size_t initial_size);

/**
 * Libère la mémoire allouée pour le Gap Buffer.
 */
void gb_destroy(GapBuffer* gb);

/**
 * Insère un caractère à la position actuelle du curseur.
 */
void gb_insert_char(GapBuffer* gb, char c);

/**
 * Insère une chaîne complète à la position du curseur.
 */
void gb_insert_string(GapBuffer* gb, const char* str);

/**
 * Supprime le caractère situé juste avant le curseur (Backspace).
 */
void gb_delete_backspace(GapBuffer* gb);

/**
 * Supprime le caractère situé juste après le curseur (Delete).
 */
void gb_delete_delete(GapBuffer* gb);

/**
 * Déplace le curseur (et donc le gap) vers la gauche ou la droite.
 * @param offset Le nombre de caractères à sauter (positif vers la droite, négatif vers la gauche).
 */
void gb_move_cursor(GapBuffer* gb, int offset);

/**
 * Récupère tout le texte actuel sous forme de chaîne terminée par '\0'.
 * @return Une nouvelle chaîne allouée dynamiquement (à libérer par l'appelant avec free()), ou NULL.
 */
char* gb_get_text(const GapBuffer* gb);

#endif // GAP_BUFFER_H
