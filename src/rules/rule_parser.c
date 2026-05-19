
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <ctype.h>
=======
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

#include "rule_parser.h"

/* ------------------------------------------------------------------ */
/* Utilitaires bas niveau                                               */
/* ------------------------------------------------------------------ */

/* Retire les espaces et guillemets autour d'une valeur JSON. */
static void trim_json_value(char *s) {
    /* Avance au-delà des espaces / guillemets de début */
    char *start = s;
    while (*start == ' ' || *start == '"' || *start == '\t')
        start++;

    /* Trouve la fin utile */
    size_t len = strlen(start);
    while (len > 0 &&
           (start[len-1] == ' ' || start[len-1] == '"' ||
            start[len-1] == ',' || start[len-1] == '\n' ||
            start[len-1] == '\r' || start[len-1] == '\t'))
        len--;

    memmove(s, start, len);
    s[len] = '\0';
}

/*
 * Extrait la valeur après la clé JSON sur la même ligne.
 * ex:  "id": "R001",   →  output = "R001"
 * ex:  "parameter": 300,  →  output = "300"
 */
static void extract_value(const char *line, char *output, size_t size) {
    const char *colon = strchr(line, ':');
    if (!colon) return;
    colon++;

    /* Saute espaces */
    while (*colon == ' ' || *colon == '\t') colon++;

    size_t i = 0;
    /* Copie jusqu'au saut de ligne ou fin de chaîne */
    while (*colon && *colon != '\n' && *colon != '\r' && i < size - 1)
        output[i++] = *colon++;
    output[i] = '\0';

    trim_json_value(output);
}

/*
 * Extrait la valeur d'un tableau JSON sur une ligne du type :
 *   "parameter": ["Résumé","Introduction", ...],
 * ou sur des lignes séparées (on accumule).
 * Les éléments sont concaténés avec une virgule comme séparateur.
 */
static void extract_array_values(
    FILE *file, const char *first_line,
    char *output, size_t size
) {
    output[0] = '\0';
    size_t used = 0;

    /* Cherche le crochet d'ouverture */
    const char *p = strchr(first_line, '[');
    if (!p) return;
    p++;

    char line[2048];
    /* On lit d'abord la suite de first_line, puis les lignes suivantes */
    int   first = 1;

    while (1) {
        const char *src;
        char tmp[2048];

        if (first) {
            /* reste de la première ligne après '[' */
            strncpy(tmp, p, sizeof(tmp) - 1);
            tmp[sizeof(tmp)-1] = '\0';
            first = 0;
            src = tmp;
        } else {
            if (!fgets(line, sizeof(line), file)) break;
            src = line;
        }

        /* Détecte la fermeture du tableau */
        const char *close = strchr(src, ']');

        /* Extrait les tokens entre guillemets sur cette ligne */
        const char *cur = src;
        while (*cur) {
            if (*cur == '"') {
                cur++;
                size_t j = 0;
                char token[256] = {0};
                while (*cur && *cur != '"' && j < sizeof(token)-1)
                    token[j++] = *cur++;
                if (*cur == '"') cur++;
                if (j > 0) {
                    if (used > 0 && used < size - 2) {
                        output[used++] = ',';
                        output[used]   = '\0';
                    }
                    size_t left = size - used - 1;
                    size_t tlen = strlen(token);
                    if (tlen > left) tlen = left;
                    memcpy(output + used, token, tlen);
                    used += tlen;
                    output[used] = '\0';
                }
            } else {
                cur++;
            }
        }

        if (close) break; /* tableau fermé */
    }
}

/*
 * Extrait la valeur d'un objet JSON inline du type :
 *   "parameter": { "level": 1, "case": "uppercase" }
 * → on retourne la chaîne brute entre accolades.
 */
static void extract_object_value(
    FILE *file, const char *first_line,
    char *output, size_t size
) {
    output[0] = '\0';
    size_t used = 0;

    const char *p = strchr(first_line, '{');
    if (!p) return;

    char line[2048];
    int first = 1;

    while (1) {
        const char *src;
        char tmp[2048];
        if (first) {
            strncpy(tmp, p, sizeof(tmp)-1);
            tmp[sizeof(tmp)-1] = '\0';
            first = 0;
            src = tmp;
        } else {
            if (!fgets(line, sizeof(line), file)) break;
            src = line;
        }

        size_t slen = strlen(src);
        size_t copy = slen;
        if (used + copy >= size) copy = size - used - 1;
        memcpy(output + used, src, copy);
        used += copy;
        output[used] = '\0';

        if (strchr(src, '}')) break;
    }
<<<<<<< HEAD

    trim_json_value(output);
}

/* ------------------------------------------------------------------ */
/* Chargement du fichier JSON                                          */
/* ------------------------------------------------------------------ */
int load_rules_from_file(const char *filename, RuleSet *ruleset) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[rule_parser] Impossible d'ouvrir : %s\n", filename);
        return 0;
    }

    ruleset->count = 0;

    char line[2048];
    Rule current;
    memset(&current, 0, sizeof(current));
    int inside_rule = 0; /* 1 quand on est dans un objet de règle */
    int depth       = 0; /* profondeur des accolades               */

    while (fgets(line, sizeof(line), file)) {

        /* ── Détection de l'objet racine et du tableau "rules" ──── */
        if (!inside_rule) {
            if (strstr(line, "\"id\"")) {
                /* Nouveau bloc règle */
                memset(&current, 0, sizeof(Rule));
                inside_rule = 1;
                depth       = 1;
                extract_value(line, current.id, sizeof(current.id));
                continue;
            }
            continue;
        }

        /* ── On est à l'intérieur d'une règle ──────────────────── */

        /* Comptage de la profondeur pour détecter la fin du bloc */
        for (const char *c = line; *c; c++) {
            if (*c == '{') depth++;
            else if (*c == '}') depth--;
        }

        if (strstr(line, "\"category\""))
            extract_value(line, current.category, sizeof(current.category));

        else if (strstr(line, "\"severity\""))
            extract_value(line, current.severity, sizeof(current.severity));

        else if (strstr(line, "\"description\""))
            extract_value(line, current.description, sizeof(current.description));

        else if (strstr(line, "\"check_type\""))
            extract_value(line, current.check_type, sizeof(current.check_type));

        else if (strstr(line, "\"parameter\"")) {
            /* Peut être : string, number, array ou object */
            const char *after_colon = strchr(line, ':');
            if (after_colon) {
                after_colon++;
                while (*after_colon == ' ' || *after_colon == '\t')
                    after_colon++;

                if (*after_colon == '[') {
                    /* Tableau → concatène les éléments avec virgule */
                    extract_array_values(file, line,
                                         current.parameter,
                                         sizeof(current.parameter));
                } else if (*after_colon == '{') {
                    /* Objet → stocke brut pour les vérificateurs avancés */
                    extract_object_value(file, after_colon,
                                          current.parameter,
                                          sizeof(current.parameter));
                } else {
                    extract_value(line, current.parameter,
                                  sizeof(current.parameter));
                }
            }
        }

        else if (strstr(line, "\"flags\""))
            extract_value(line, current.flags, sizeof(current.flags));

        else if (strstr(line, "\"target_section\""))
            extract_value(line, current.target_section,
                          sizeof(current.target_section));

        /* ── Fin du bloc règle ─────────────────────────────────── */
        if (depth <= 0) {
            if (ruleset->count < MAX_RULES)
                ruleset->rules[ruleset->count++] = current;
            else
                fprintf(stderr,
                        "[rule_parser] MAX_RULES (%d) atteint, règle ignorée\n",
                        MAX_RULES);

            memset(&current, 0, sizeof(Rule));
            inside_rule = 0;
            depth       = 0;
        }
    }

=======
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
    }
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
    fclose(file);
    return 1;
}

/* ------------------------------------------------------------------ */
/* Debug : affichage du RuleSet chargé                                 */
/* ------------------------------------------------------------------ */
void print_ruleset(const RuleSet *ruleset) {
    printf("[rule_parser] %d règle(s) chargée(s)\n", ruleset->count);
    for (int i = 0; i < ruleset->count; i++) {
        const Rule *r = &ruleset->rules[i];
        printf("  [%s] (%s / %s) check=%s  param=\"%s\"\n",
               r->id, r->category, r->severity,
               r->check_type, r->parameter);
    }
<<<<<<< HEAD
=======

    free(ruleset->rules);
    free(ruleset);
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
}
