#include "history.h"
#include <stdlib.h>
#include <string.h>

History* history_create(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 16;
    
    History* h = (History*)malloc(sizeof(History));
    if (!h) return NULL;
    
    h->actions = (EditorAction*)malloc(initial_capacity * sizeof(EditorAction));
    if (!h->actions) {
        free(h);
        return NULL;
    }
    
    h->count = 0;
    h->capacity = initial_capacity;
    h->current_index = 0;
    
    return h;
}

void history_destroy(History* h) {
    if (h) {
        if (h->actions) {
            for (size_t i = 0; i < h->count; i++) {
                if (h->actions[i].text) {
                    free(h->actions[i].text);
                }
            }
            free(h->actions);
        }
        free(h);
    }
}

void history_record(History* h, ActionType type, size_t position, const char* text) {
    if (!h || !text) return;
    
    // Si nous étions revenus dans le passé, on efface le futur avant d'enregistrer.
    for (size_t i = h->current_index; i < h->count; i++) {
        if (h->actions[i].text) {
            free(h->actions[i].text);
            h->actions[i].text = NULL;
        }
    }
    h->count = h->current_index;
    
    if (h->count >= h->capacity) {
        h->capacity *= 2;
        EditorAction* new_actions = (EditorAction*)realloc(h->actions, h->capacity * sizeof(EditorAction));
        if (!new_actions) return; // Erreur mémoire
        h->actions = new_actions;
    }
    
    h->actions[h->count].type = type;
    h->actions[h->count].position = position;
    h->actions[h->count].text = strdup(text);
    
    h->count++;
    h->current_index++;
}

const EditorAction* history_undo(History* h) {
    if (!h || h->current_index == 0) return NULL;
    
    h->current_index--;
    return &h->actions[h->current_index];
}

const EditorAction* history_redo(History* h) {
    if (!h || h->current_index >= h->count) return NULL;
    
    const EditorAction* action = &h->actions[h->current_index];
    h->current_index++;
    return action;
}
