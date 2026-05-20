#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/editor/editor_core.h"
#include "include/gap_buffer.h"
#include "include/tokenizer.h"
#include "include/spellcheck.h"

int main() {
    char input[256];
    printf("=== IntelliEditor — Mode Correction Realiste ===\n");
    printf("Initialisation de l'editeur de texte et du dictionnaire...\n");
    
    // 1. Initialisation de l'éditeur
    EditorContext* ctx = editor_create(1024);
    if (!ctx) {
        printf("Erreur d'initialisation de l'editeur.\n");
        return 1;
    }

    // 2. Initialisation du dictionnaire du projet
    if (!spellcheck_init("./dictionnaire_fr.txt")) {
        printf("Erreur : Impossible de charger le fichier dictionnaire_fr.txt\n");
        editor_destroy(ctx);
        return 1;
    }
    printf("Editeur et dictionnaire francais prets !\n\n");

    // 3. Boucle interactive
    printf("Tapez un mot ou une phrase (ex: 'bondoir' ou 'salu') : ");
    if (fgets(input, sizeof(input), stdin)) {
        // Enlever le retour à la ligne '\n'
        input[strcspn(input, "\n")] = 0;

        printf("\n[Analyse] Verification de : \"%s\"\n", input);
        
        // On utilise la vraie fonction pour vérifier l'orthographe
        if (spellcheck_is_correct(input)) {
            printf("[Succes] Le mot \"%s\" est correctement orthographie !\n", input);
        } else {
            printf("[Alerte] Mot mal orthographie detecte.\n");
            printf("Recherche de suggestions via l'algorithme de Levenshtein...\n");
            
            // On appelle les vraies structures et fonctions de suggestion
            SpellSuggestions suggestions = spellcheck_get_suggestions(input);
            
            if (suggestions.count > 0) {
                printf(">> Suggestions trouvees (%d) :\n", suggestions.count);
                for (int i = 0; i < suggestions.count; i++) {
                    printf("   - %s\n", suggestions.words[i]);
                }
            } else {
                printf(">> Aucune suggestion proche trouvee dans le dictionnaire.\n");
            }
        }
    }

    // 4. Nettoyage propre de la mémoire
    spellcheck_cleanup();
    editor_destroy(ctx);

    return 0;
}