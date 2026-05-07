#ifndef LLM_THREAD_H
#define LLM_THREAD_H
#include <stdbool.h>

// Structure pour configurer l'IA
typedef struct {
    const char *model_path;   // Chemin vers mon fichier llama-1b.gguf
    int n_threads;            // Nombre de coeurs CPU à utiliser
} llm_config_t;

// --- Fonctions que l' éditeur de texte utilisera ---

// 1. Initialise et lance le "Worker Thread" en arrière-plan
bool llm_thread_init(llm_config_t config);

// 2. Envoie une question à l'IA (Asynchrone : ne bloque pas l'écran)
void llm_thread_query(const char *prompt);

// 3. Récupère le texte généré par l'IA (petit à petit)
// Retourne true si un nouveau mot est disponible
bool llm_thread_get_response(char *buffer, int buffer_size);

// 4. Vérifie si l'IA est encore en train de travailler
bool llm_thread_is_busy();

// 5. Arrête proprement le thread et libère la mémoire
void llm_thread_stop();

#endif // LLM_THREAD_H