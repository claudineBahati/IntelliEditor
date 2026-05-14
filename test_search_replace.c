#include "gap_buffer.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("Test search_replace...\n");

    GapBuffer* gb = gb_create(32);
    if (!gb) {
        printf("Erreur création gap buffer\n");
        return 1;
    }

    gb_insert_string(gb, "Hello World!");
    printf("Texte initial: %s\n", gb_get_text(gb));

    size_t replace_pos = search_replace(gb, "World", "IntelliEditor", 0, true);
    if (replace_pos == (size_t)-1) {
        printf("Erreur remplacement\n");
        gb_destroy(gb);
        return 1;
    }

    char* result = gb_get_text(gb);
    printf("Texte après remplacement: %s\n", result);
    printf("Résultat attendu: Hello IntelliEditor!\n");

    bool success = strcmp(result, "Hello IntelliEditor!") == 0;
    printf("Test %s\n", success ? "RÉUSSI" : "ÉCHOUÉ");

    free(result);
    gb_destroy(gb);

    return success ? 0 : 1;
}