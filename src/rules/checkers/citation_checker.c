#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "citation_checker.h"

/* ------------------------------------------------------------------ */
/* Recherche insensible à la casse                                     */
/* ------------------------------------------------------------------ */
static int strcasestr_local(const char *haystack, const char *needle) {
    if (!*needle) return 1;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n &&
               tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++; n++;
        }
        if (!*n) return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* check_citation_present                                              */
/* ------------------------------------------------------------------ */
int check_citation_present(const char *document, const char *parameter) {
    if (!document || !parameter || !*parameter) return 0;

    char buf[256];
    strncpy(buf, parameter, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    /* Tente chaque alternative séparée par '|' */
    char *tok = strtok(buf, "|");
    while (tok) {
        while (*tok == ' ') tok++;
        char *end = tok + strlen(tok) - 1;
        while (end > tok && *end == ' ') *end-- = '\0';

        if (strcasestr_local(document, tok))
            return 1;

        tok = strtok(NULL, "|");
    }
    return 0;
}
