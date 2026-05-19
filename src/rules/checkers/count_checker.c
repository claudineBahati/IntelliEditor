#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "count_checker.h"

/* ------------------------------------------------------------------ */
/* Comptage de mots                                                     */
/* ------------------------------------------------------------------ */
int count_words(const char *text) {
    if (!text) return 0;

    int  words    = 0;
    int  in_word  = 0;

    while (*text) {
        if (isspace((unsigned char)*text)) {
            in_word = 0;
        } else if (!in_word) {
            words++;
            in_word = 1;
        }
        text++;
    }
    return words;
}

int check_word_count_min(const char *text, int min_words) {
    return count_words(text) >= min_words;
}

int check_word_count_max(const char *text, int max_words) {
    return count_words(text) <= max_words;
}

/* ------------------------------------------------------------------ */
/* Extraction de la valeur numérique depuis le paramètre JSON          */
/*                                                                      */
/* Deux formes possibles :                                              */
/*   1) simple : "300"                                                  */
/*   2) objet  : { "section": "Introduction", "min_words": 300 }       */
/*               { "section": "Résumé",       "max_words": 250 }       */
/* ------------------------------------------------------------------ */

static int extract_number_from_key(const char *param, const char *key) {
    const char *p = strstr(param, key);
    if (!p) return 0;

    p += strlen(key);

    /* Cherche ':' ou espace puis le nombre */
    while (*p && (*p == ':' || *p == ' ' || *p == '\t' || *p == '"'))
        p++;

    if (*p == '\0') return 0;
    return atoi(p);
}

int extract_min_words(const char *parameter) {
    if (!parameter || !*parameter) return 0;

    /* Forme objet JSON → cherche "min_words" */
    if (strchr(parameter, '{')) {
        int v = extract_number_from_key(parameter, "\"min_words\"");
        if (v > 0) return v;
        /* Fallback : cherche juste un nombre */
    }
    return atoi(parameter);
}

int extract_max_words(const char *parameter) {
    if (!parameter || !*parameter) return 0;

    /* Forme objet JSON → cherche "max_words" */
    if (strchr(parameter, '{')) {
        int v = extract_number_from_key(parameter, "\"max_words\"");
        if (v > 0) return v;
    }
    return atoi(parameter);
}
