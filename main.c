#include <stdio.h>
#include <windows.h>
#include "llm/llm_thread.h"

int main() {
    printf("=== TEST DE LA FILE D'ATTENTE (QUEUE) ===\n");

    // 1. Initialisation
    llm_config_t config = {"models/llama-1b.gguf", 4};
    if (!llm_thread_init(config)) {
        printf("Erreur d'initialisation !\n");
        return 1;
    }

    // 2. On envoie 3 requêtes très vite
    printf("[Main] Envoi de 3 requetes dans la file...\n");
    llm_thread_query("Quelle est la capitale de la France ?");
    llm_thread_query("Combien font 2 + 2 ?");
    llm_thread_query("Qui est Albert Einstein ?");

    // 3. Boucle de surveillance
    // On attend tant que la file n'est pas vide
    while (llm_thread_is_busy()) {
        printf("[Main] L'interface est FLUIDE. L'IA travaille en arriere-plan...\n");
        Sleep(1500); // On attend 1.5 seconde entre chaque vérification
    }

    printf("\n[Main] Toutes les requetes ont ete traitees.\n");

    // 4. Nettoyage
    llm_thread_stop();
    printf("Programme termine proprement.\n");

    return 0;
}