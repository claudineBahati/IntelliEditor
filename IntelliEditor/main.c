#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "llm/llm_thread.h"
#include "nlp/nlp_engine.h" // On inclut uniquement le moteur maintenant
#include "llm/json_parser/json_parser.h"

int main() {
    // 1. Initialisation du LLM
    llm_config_t config = {0}; 
    if (!llm_thread_init(config)) {
        printf("Erreur lors de l'initialisation du thread LLM\n");
        return 1;
    }
    char *fake_json = "{\"choices\": [{\"message\": {\"content\": \"Ceci est une correction reussie !\"}}]}";
    char result[256];

    if (parse_llm_response(fake_json, result, sizeof(result))) {
    printf("\n[TEST JSON] Succes ! Contenu extrait : %s\n", result);
    } else {
    printf("\n[TEST JSON] Echec du parsing.\n");
   }

    // 2. Initialisation du Correcteur (NLP)
    // On garde l'anglais pour l'instant comme ton test a fonctionné
    if (!nlp_init("dictionaries/en.aff", "dictionaries/en.dic")) {
        printf("Attention : Dictionnaires non trouves.\n");
    } else {
        printf("[NLP] Moteur de correction pret.\n");
    }

    printf("=== IntelliEditor v1.0 ===\n");
    printf("Tapez votre texte (le thread LLM et le moteur NLP travaillent ensemble).\n");

    char buffer[1024];
    while (1) {
        printf("\nEntrez un texte (ou 'exit' pour quitter) : \n> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "exit") == 0) break;

        // --- NOUVELLE PARTIE : UTILISATION DU MOTEUR ---
        // Le nlp_engine s'occupe de tout (Tokenize + Check)
        CorrectionReport report = nlp_process_text(buffer);
        
        printf("[NLP] Analyse terminee : %d faute(s) trouvee(s).\n", report.error_count);
        for (int i = 0; i < report.error_count; i++) {
            printf("  -> Faute : %s (Mot n°%d)\n", report.errors[i].word, report.errors[i].word_index + 1);
        }
        
        // On libère la mémoire du rapport (très important !)
        nlp_free_report(&report);

        // --- PARTIE LLM ---
        llm_thread_query(buffer);
        printf("[Main] Requete envoyee au thread LLM...\n");
    }

    // 3. Fermeture propre
    llm_thread_stop();
    nlp_cleanup(); 
    printf("Programme arrete.\n");

    return 0;
}