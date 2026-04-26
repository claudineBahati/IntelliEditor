#include "gap_buffer.h"
#include <stdlib.h>
#include <string.h>

// Déclaration interne
static void gb_expand(GapBuffer* gb, size_t added_size);

GapBuffer* gb_create(size_t initial_size) {
    if (initial_size == 0) initial_size = 64; // Taille par défaut raisonnable

    GapBuffer* gb = (GapBuffer*)malloc(sizeof(GapBuffer));
    if (!gb) return NULL;

    gb->buffer = (char*)malloc(initial_size);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }

    gb->size = initial_size;
    gb->gap_start = 0;
    gb->gap_end = initial_size;

    return gb;
}

void gb_destroy(GapBuffer* gb) {
    if (gb) {
        if (gb->buffer) free(gb->buffer);
        free(gb);
    }
}

static void gb_expand(GapBuffer* gb, size_t added_size) {
    // Calculer la nouvelle taille
    size_t new_size = gb->size * 2 + added_size;
    
    // Réallouer
    char* new_buffer = (char*)realloc(gb->buffer, new_size);
    if (!new_buffer) return; // Gestion d'erreur basique
    gb->buffer = new_buffer;

    // Déplacer le texte qui est après le gap vers la toute fin du nouveau buffer
    size_t post_gap_len = gb->size - gb->gap_end;
    size_t new_gap_end = new_size - post_gap_len;
    
    // Utiliser memmove car les zones pourraient (en théorie) se chevaucher
    memmove(gb->buffer + new_gap_end, gb->buffer + gb->gap_end, post_gap_len);

    gb->gap_end = new_gap_end;
    gb->size = new_size;
}

void gb_insert_char(GapBuffer* gb, char c) {
    if (!gb) return;

    if (gb->gap_start == gb->gap_end) {
        gb_expand(gb, 1);
    }

    gb->buffer[gb->gap_start] = c;
    gb->gap_start++;
}

void gb_insert_string(GapBuffer* gb, const char* str) {
    if (!gb || !str) return;

    size_t len = strlen(str);
    size_t gap_len = gb->gap_end - gb->gap_start;

    if (gap_len < len) {
        gb_expand(gb, len);
    }

    memcpy(gb->buffer + gb->gap_start, str, len);
    gb->gap_start += len;
}

void gb_delete_backspace(GapBuffer* gb) {
    if (!gb) return;
    
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

void gb_delete_delete(GapBuffer* gb) {
    if (!gb) return;

    if (gb->gap_end < gb->size) {
        gb->gap_end++;
    }
}

void gb_move_cursor(GapBuffer* gb, int offset) {
    if (!gb || offset == 0) return;

    if (offset > 0) {
        // Déplacement vers la droite
        size_t steps = (size_t)offset;
        size_t post_gap_len = gb->size - gb->gap_end;
        if (steps > post_gap_len) steps = post_gap_len;

        // On transfère les caractères de droite du gap vers la gauche
        memmove(gb->buffer + gb->gap_start, gb->buffer + gb->gap_end, steps);
        gb->gap_start += steps;
        gb->gap_end += steps;
    } else {
        // Déplacement vers la gauche
        size_t steps = (size_t)(-offset);
        if (steps > gb->gap_start) steps = gb->gap_start;

        // On transfère les caractères de la gauche du gap vers la droite
        gb->gap_start -= steps;
        gb->gap_end -= steps;
        memmove(gb->buffer + gb->gap_end, gb->buffer + gb->gap_start, steps);
    }
}

char* gb_get_text(const GapBuffer* gb) {
    if (!gb) return NULL;

    size_t pre_gap_len = gb->gap_start;
    size_t post_gap_len = gb->size - gb->gap_end;
    size_t total_len = pre_gap_len + post_gap_len;

    char* text = (char*)malloc(total_len + 1);
    if (!text) return NULL;

    // Copier la partie avant le gap
    if (pre_gap_len > 0) {
        memcpy(text, gb->buffer, pre_gap_len);
    }
    
    // Copier la partie après le gap
    if (post_gap_len > 0) {
        memcpy(text + pre_gap_len, gb->buffer + gb->gap_end, post_gap_len);
    }

    text[total_len] = '\0';
    return text;
}
