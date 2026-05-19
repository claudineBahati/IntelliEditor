#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nlp/nlp_engine.h"
#include "llm/llm_interface.h"

int main() {
    // 1. Initialisation Hunspell
    printf("[DEBUG] Initialisation Hunspell...\n");
    if (!nlp_init("dictionaries/en.aff", "dictionaries/en.dic")) {
        printf("ERREUR : Dictionnaires introuvables dans le dossier 'dictionaries/'\n");
        return 1;
    }

    // 2. Initialisation LLM
    
    LLMConfig config = {
        .model_path = "models/llama-1b.gguf", 
        .n_ctx = 512,
        .n_batch = 130,
        .n_threads = 4
    };

    printf("[DEBUG] Chargement du modele IA : %s\n", config.model_path);
    if (!llm_init(config)) {
        printf("ERREUR : Impossible de charger le modele. Verifiez le chemin du fichier .gguf !\n");
        nlp_cleanup();
        return 1;
    }
    printf("[DEBUG] IA chargee avec succes.\n");

    char input[512]; 
    printf("\n=== INTELLIEDITOR PRET ===\n> ");

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "exit") == 0) break;

        printf("--- Analyse ---\n");
        
        // CORRECTIONS : On appelle DIRECTEMENT l'IA sans passer par le nlp_engine anglais
        char* reponse_ia = llm_generate_paraphrase(input);
        
        if (reponse_ia != NULL && strlen(reponse_ia) > 0) {
            printf("Alternatives :\n-%s\n", reponse_ia);
            free(reponse_ia); // Libération de la mémoire
        } else {
            printf("Aucune suggestion.\n");
        }
        
        printf("\n> ");
    }
        

    llm_cleanup();
    nlp_cleanup();
    return 0;
}