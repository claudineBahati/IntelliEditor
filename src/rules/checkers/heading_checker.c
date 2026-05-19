#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "heading_checker.h"

/* ------------------------------------------------------------------ */
/* Utilitaires                                                          */
/* ------------------------------------------------------------------ */

/*
 * Extrait la valeur d'une clé JSON simple dans un objet inline.
 * ex:  { "level": 1, "case": "uppercase" }
 *      extract_json_string(param, "case", buf, size) → "uppercase"
 */
static void extract_json_string(const char *json, const char *key,
                                 char *out, size_t size) {
    out[0] = '\0';
    char search[64];
    snprintf(search, sizeof(search), "\"%s\"", key);

    const char *p = strstr(json, search);
    if (!p) return;
    p += strlen(search);
    while (*p == ' ' || *p == ':' || *p == '\t') p++;

    if (*p == '"') {
        p++;
        size_t i = 0;
        while (*p && *p != '"' && i < size - 1)
            out[i++] = *p++;
        out[i] = '\0';
    } else {
        /* Valeur numérique */
        size_t i = 0;
        while (*p && *p != ',' && *p != '}' && *p != ' ' && i < size - 1)
            out[i++] = *p++;
        out[i] = '\0';
    }
}

/*
 * Construit le préfixe Markdown correspondant au niveau de titre.
 * level=1 → "# ", level=2 → "## ", etc.
 */
static void heading_prefix(int level, char *buf, size_t size) {
    buf[0] = '\0';
    if (level < 1 || level > 4) return;
    for (int i = 0; i < level && i < (int)size - 2; i++)
        buf[i] = '#';
    buf[level] = ' ';
    buf[level + 1] = '\0';
}

/* ------------------------------------------------------------------ */
/* Vérifications de casse                                              */
/* ------------------------------------------------------------------ */

static int is_uppercase(const char *s) {
    int has_alpha = 0;
    for (; *s; s++) {
        if (isalpha((unsigned char)*s)) {
            has_alpha = 1;
            if (islower((unsigned char)*s)) return 0;
        }
    }
    return has_alpha; /* 0 si aucune lettre */
}

static int is_lowercase(const char *s) {
    int has_alpha = 0;
    for (; *s; s++) {
        if (isalpha((unsigned char)*s)) {
            has_alpha = 1;
            if (isupper((unsigned char)*s)) return 0;
        }
    }
    return has_alpha;
}

static int is_capitalized(const char *s) {
    /* Première lettre majuscule, reste quelconque */
    while (*s && !isalpha((unsigned char)*s)) s++;
    if (!*s) return 0;
    return isupper((unsigned char)*s);
}

/* ------------------------------------------------------------------ */
/* check_heading_format                                                 */
/* ------------------------------------------------------------------ */
int check_heading_format(const char *document, const char *parameter) {
    if (!document || !parameter || !*parameter) return 1;

    /* Lecture des paramètres */
    char level_str[8]  = {0};
    char format_str[32] = {0};

    extract_json_string(parameter, "level",  level_str,  sizeof(level_str));
    extract_json_string(parameter, "case",   format_str, sizeof(format_str));

    int level = atoi(level_str);
    if (level < 1 || level > 4) return 1;

    char prefix[8];
    heading_prefix(level, prefix, sizeof(prefix));
    size_t prefix_len = strlen(prefix);

    /* Parcours ligne par ligne */
    char line[1024];
    const char *p   = document;
    int   checked   = 0; /* nombre de titres du bon niveau trouvés */
    int   violated  = 0;

    while (*p) {
        /* Copie une ligne */
        size_t i = 0;
        while (*p && *p != '\n' && i < sizeof(line) - 1)
            line[i++] = *p++;
        line[i] = '\0';
        if (*p == '\n') p++;

        /* Vérifie si c'est un titre du bon niveau */
        if (strncmp(line, prefix, prefix_len) != 0) continue;

        /* Assure que ce n'est pas un titre de niveau supérieur
           (ex: "## " ne doit pas matcher "#") */
        if (level < 4 && line[prefix_len] == '#') continue;

        const char *title_text = line + prefix_len;
        checked++;

        int ok = 0;
        if (strcmp(format_str, "uppercase") == 0)
            ok = is_uppercase(title_text);
        else if (strcmp(format_str, "lowercase") == 0)
            ok = is_lowercase(title_text);
        else if (strcmp(format_str, "capitalize") == 0)
            ok = is_capitalized(title_text);
        else
            ok = 1; /* format inconnu → neutre */

        if (!ok) violated++;
    }

    if (checked == 0) return 1; /* aucun titre de ce niveau → OK neutre */
    return violated == 0;
}
