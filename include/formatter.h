#ifndef FORMATTER_H
#define FORMATTER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Représente un style de texte (couleurs, attributs).
 * Note: Les couleurs sont au format 0x00RRGGBB.
 */
typedef struct {
    uint32_t fg_color;
    uint32_t bg_color;
    bool bold;
    bool italic;
    bool underline;
} TextStyle;

/**
 * Une plage de texte avec un style spécifique.
 */
typedef struct {
    size_t start;
    size_t length;
    TextStyle style;
} StyleRange;

/**
 * Structure principale du moteur de formatage.
 */
typedef struct {
    StyleRange* ranges;
    size_t count;
    size_t capacity;
} Formatter;

/**
 * Crée un nouveau moteur de formatage.
 */
Formatter* formatter_create(size_t initial_capacity);

/**
 * Libère le moteur de formatage.
 */
void formatter_destroy(Formatter* f);

/**
 * Ajoute une plage stylisée.
 */
void formatter_add_range(Formatter* f, size_t start, size_t length, TextStyle style);

/**
 * Efface toutes les plages (ex: avant une re-coloration syntaxique).
 */
void formatter_clear_ranges(Formatter* f);

/**
 * Récupère le style à une position donnée.
 * Retourne true si un style spécifique a été trouvé.
 */
bool formatter_get_style_at(const Formatter* f, size_t pos, TextStyle* out_style);

#endif // FORMATTER_H
