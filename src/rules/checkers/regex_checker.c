#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "regex_checker.h"

/* ------------------------------------------------------------------ */
/* Utilitaires                                                          */
/* ------------------------------------------------------------------ */

static int is_case_insensitive(const char *flags) {
    if (!flags || !*flags) return 0;
    return strstr(flags, "case_insensitive") != NULL;
}

/*
 * Vérifie si *text* contient *pattern* (séquence exacte de caractères).
 * Si ignore_case == 1, la comparaison est insensible à la casse.
 */
static int contains(const char *text, const char *pattern, int ignore_case) {
    if (!text || !pattern || !*pattern) return 0;

    for (; *text; text++) {
        const char *t = text;
        const char *p = pattern;
        while (*t && *p) {
            char tc = ignore_case ? (char)tolower((unsigned char)*t) : *t;
            char pc = ignore_case ? (char)tolower((unsigned char)*p) : *p;
            if (tc != pc) break;
            t++;
            p++;
        }
        if (!*p) return 1;
    }
    return 0;
}

/*
 * Vérifie si *text* contient le token *pattern* en tant que mot entier
 * (entouré de non-alphanumériques ou aux bords).
 */
static int contains_word(const char *text, const char *pattern,
                          int ignore_case) {
    if (!text || !pattern || !*pattern) return 0;

    size_t plen = strlen(pattern);

    for (const char *t = text; *t; t++) {
        /* Vérifie le début de mot */
        if (t > text && (isalnum((unsigned char)*(t-1)) ||
                         *(t-1) == '_'))
            continue;

        const char *h = t;
        const char *p = pattern;
        while (*h && *p) {
            char hc = ignore_case ? (char)tolower((unsigned char)*h) : *h;
            char pc = ignore_case ? (char)tolower((unsigned char)*p) : *p;
            if (hc != pc) break;
            h++;
            p++;
        }
        if (!*p) {
            /* Vérifie la fin de mot */
            if (!*h || (!isalnum((unsigned char)*h) && *h != '_'))
                return 1;
        }
    }
    (void)plen;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Gestion des motifs avec alternance  (a|b|c)                         */
/* Le parser gère aussi \b (frontière de mot) de façon simplifiée.     */
/* ------------------------------------------------------------------ */

/*
 * Teste si le motif *tok* matche à une position du texte.
 * \b est interprété comme "frontière de mot".
 * Retourne 1 si trouvé.
 */
static int match_token(const char *text, const char *tok, int ignore_case) {
    /* Retire les \b de début/fin pour déterminer le type de match */
    int word_boundary_start = 0;
    int word_boundary_end   = 0;

    char clean[256] = {0};
    size_t ci = 0;

    for (const char *p = tok; *p && ci < sizeof(clean)-1; p++) {
        if (*p == '\\' && *(p+1) == 'b') {
            if (p == tok)       word_boundary_start = 1;
            else                word_boundary_end   = 1;
            p++; /* saute 'b' */
        } else {
            clean[ci++] = *p;
        }
    }
    clean[ci] = '\0';

    if (!*clean) return 0;

    if (word_boundary_start || word_boundary_end)
        return contains_word(text, clean, ignore_case);

    return contains(text, clean, ignore_case);
}

/*
 * Teste un motif pouvant contenir des alternatives séparées par '|'.
 * Retourne 1 si au moins une alternative est trouvée.
 */
static int match_pattern(const char *text, const char *pattern,
                          int ignore_case) {
    /* Copie modifiable */
    char buf[512];
    strncpy(buf, pattern, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok = strtok(buf, "|");

    while (tok) {
        /* Retire les espaces */
        while (*tok == ' ') tok++;
        char *end = tok + strlen(tok) - 1;
        while (end > tok && *end == ' ') *end-- = '\0';

        if (match_token(text, tok, ignore_case))
            return 1;

        tok = strtok(NULL, "|");
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* API publique                                                         */
/* ------------------------------------------------------------------ */

int check_forbidden_pattern(const char *text,
                              const char *pattern,
                              const char *flags) {
    return match_pattern(text, pattern, is_case_insensitive(flags));
}

int check_required_pattern(const char *text,
                             const char *pattern,
                             const char *flags) {
    return match_pattern(text, pattern, is_case_insensitive(flags));
}

/* Aliases */
int check_forbidden_word(const char *text, const char *word) {
    return check_forbidden_pattern(text, word, NULL);
}

int check_required_expression(const char *text, const char *expression) {
    return check_required_pattern(text, expression, NULL);
}
