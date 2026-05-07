#include "formatter.h"
#include "debug_memory.h"
#include <stdlib.h>
#include <string.h>

Formatter* formatter_create(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 16;
    
    Formatter* f = (Formatter*)malloc(sizeof(Formatter));
    if (!f) return NULL;
    
    f->ranges = (StyleRange*)malloc(initial_capacity * sizeof(StyleRange));
    if (!f->ranges) {
        free(f);
        return NULL;
    }
    
    f->count = 0;
    f->capacity = initial_capacity;
    
    return f;
}

void formatter_destroy(Formatter* f) {
    if (f) {
        if (f->ranges) free(f->ranges);
        free(f);
    }
}

void formatter_add_range(Formatter* f, size_t start, size_t length, TextStyle style) {
    if (!f) return;
    
    if (f->count >= f->capacity) {
        f->capacity *= 2;
        StyleRange* new_ranges = (StyleRange*)realloc(f->ranges, f->capacity * sizeof(StyleRange));
        if (!new_ranges) return;
        f->ranges = new_ranges;
    }
    
    f->ranges[f->count].start = start;
    f->ranges[f->count].length = length;
    f->ranges[f->count].style = style;
    f->count++;
}

void formatter_clear_ranges(Formatter* f) {
    if (f) f->count = 0;
}

bool formatter_get_style_at(const Formatter* f, size_t pos, TextStyle* out_style) {
    if (!f || !out_style) return false;
    
    // On parcourt les plages (la dernière ajoutée a la priorité, typiquement)
    // Pour l'instant, on prend la première qui matche.
    for (size_t i = 0; i < f->count; i++) {
        if (pos >= f->ranges[i].start && pos < f->ranges[i].start + f->ranges[i].length) {
            *out_style = f->ranges[i].style;
            return true;
        }
    }
    
    return false;
}
