#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nlp/nlp_engine.h"

int main() {
    // 1. INITIALISATION (On le fait AU DÉBUT, une seule fois)
    if (!nlp_init("dictionaries/en.aff", "dictionaries/en.dic")) {
        printf("Erreur de chargement des dictionnaires !\n");
        return 1;
    }

    // Déclaration de la variable qui va stocker la saisie
    char input[512]; 

    printf("=== INTELLIEDITOR - MODE INTERACTIF ===\n");
    printf("Tapez une phrase ou 'exit' pour quitter.\n");

    // 2. LA BOUCLE INTERACTIVE
    while (1) {
        printf("\n> ");
        
        // Lire l'entrée clavier
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        // Enlever le retour à la ligne (\n)
        input[strcspn(input, "\n")] = 0;

        // Quitter si on tape "exit"
        if (strcmp(input, "exit") == 0) break;
        if (strlen(input) == 0) continue;

        printf("--- Analyse en cours ---\n");

        // 3. ANALYSE (Orthographe et Sémantique)
        CorrectionReport report = nlp_process_text(input);

        // 4. AFFICHAGE ORTHOGRAPHE
        printf("\n[Orthographe] : %d faute(s) trouvee(s).\n", report.sp_error_count);
        for (int i = 0; i < report.sp_error_count; i++) {
            printf("  -> Mot inconnu : %s\n", report.sp_errors[i].word);
        }

        // 5. AFFICHAGE GRAMMAIRE / SÉMANTIQUE
        if (report.gr_error_count > 0) {
            printf("\n[Grammaire/IA] : %d alerte(s).\n", report.gr_error_count);
            for (int i = 0; i < report.gr_error_count; i++) {
                printf("  -> %s : %s\n", report.gr_errors[i].error_type, report.gr_errors[i].suggested_correction);
            }
        }

        // 6. REFORMULATION
        printf("\n[Reformulation] : Suggestions de style\n");
        ParaphraseReport para_report = nlp_generate_paraphrase(input);

        if (para_report.suggestion_count > 0) {
            for (int i = 0; i < para_report.suggestion_count; i++) {
                printf("  -> Option %d : %s\n", i + 1, para_report.suggestions[i]);
            }
        } else {
            printf("  -> Aucune suggestion.\n");
        }

        // 7. NETTOYAGE pour la prochaine boucle
        nlp_free_report(&report);
    }

    // 8. FERMETURE FINALE
    nlp_cleanup();
    printf("\n=== FIN DU PROGRAMME ===\n");
    return 0;
}