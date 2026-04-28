#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "llm/llm_thread.h" // Vérifie que le chemin est bon

int main() {
    // 1. Initialisation (on utilise une config vide pour l'instant)
    llm_config_t config = {0}; 
    if (!llm_thread_init(config)) {
        printf("Erreur lors de l'initialisation du thread LLM\n");
        return 1;
    }

    printf("=== IntelliEditor v1.0 ===\n");
    printf("Tapez votre texte et le thread s'en occupera en arriere-plan.\n");

    char buffer[1024];
    while (1) {
        printf("\nEntrez un texte (ou 'exit' pour quitter) : \n> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        
        buffer[strcspn(buffer, "\n")] = 0; // Nettoyer le \n

        if (strcmp(buffer, "exit") == 0) break;

        // 2. On envoie la requete via ton API
        llm_thread_query(buffer);
        
        printf("[Main] Requete envoyee au thread...\n");
    }

    // 3. On ferme proprement
    llm_thread_stop();
    printf("Programme arrete.\n");

    return 0;
}