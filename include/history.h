#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    ACTION_INSERT,
    ACTION_DELETE
} ActionType;

typedef struct {
    ActionType type;
    size_t position; // L'endroit où l'action s'est produite
    char* text;      // Le texte inséré ou supprimé
} EditorAction;

typedef struct {
    EditorAction* actions;
    size_t count;
    size_t capacity;
    size_t current_index; // Index actuel pour annuler/refaire
} History;

/**
 * Crée un nouvel historique.
 */
History* history_create(size_t initial_capacity);

/**
 * Libère l'historique et toutes les chaînes associées.
 */
void history_destroy(History* h);

/**
 * Enregistre une nouvelle action dans l'historique.
 * Attention: L'enregistrement d'une nouvelle action écrase tout le futur (les "Refaire" potentiels).
 */
void history_record(History* h, ActionType type, size_t position, const char* text);

/**
 * Récupère l'action précédente à annuler (Undo).
 * Décrémente l'index actuel.
 * @return Un pointeur vers l'action à annuler, ou NULL si rien à annuler.
 */
const EditorAction* history_undo(History* h);

/**
 * Récupère l'action suivante à refaire (Redo).
 * Incrémente l'index actuel.
 * @return Un pointeur vers l'action à refaire, ou NULL si rien à refaire.
 */
const EditorAction* history_redo(History* h);

#endif // HISTORY_H
