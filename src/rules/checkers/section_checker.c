#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "section_checker.h"

/* ------------------------------------------------------------------ */
/* Utilitaires internes                                                 */
/* ------------------------------------------------------------------ */

/*
 * Recherche insensible à la casse de *needle* dans *haystack*.
 * Retourne un pointeur vers la première occurrence ou NULL.
 */
static const char *strcasestr_compat(const char *haystack,
                                     const char *needle) {
    if (!*needle) return haystack;

    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && tolower((unsigned char)*h) ==
                           tolower((unsigned char)*n)) {
            h++;
            n++;
        }
        if (!*n) return haystack;
    }
    return NULL;
}

/* ------------------------------------------------------------------ */
/* check_section_exists                                                 */
/* ------------------------------------------------------------------ */
int check_section_exists(const char *document, const char *section) {
    if (!document || !section || !*section) return 0;
    return strcasestr_compat(document, section) != NULL;
}

/* ------------------------------------------------------------------ */
/* check_section_order                                                  */
/* ------------------------------------------------------------------ */
int check_section_order(const char *document, const char *parameter) {
    if (!document || !parameter || !*parameter) return 1;

    /* On travaille sur une copie modifiable */
    char buf[512];
    strncpy(buf, parameter, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    const char *last_pos = NULL; /* pointeur dans le document */
    char *token = strtok(buf, ",");

    while (token) {
        /* Retire les espaces autour du token */
        while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') *end-- = '\0';

        const char *found = strcasestr_compat(document, token);

        if (found) {
            if (last_pos && found < last_pos)
                return 0; /* ordre incorrect */
            last_pos = found;
        }
        /* Si la section n'est pas trouvée on ne pénalise pas l'ordre */

        token = strtok(NULL, ",");
    }

    return 1;
}
