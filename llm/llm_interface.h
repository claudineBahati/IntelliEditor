#ifndef LLM_INTERFACE_H
#define LLM_INTERFACE_H

#include <stdbool.h>

// Structure de configuration pour l'optimisation
typedef struct {
    const char* model_path;
    int n_ctx;      // Taille du cache KV (ex: 512)
    int n_batch;    // Taille du batch pour la vitesse (ex: 32)
    int n_threads;  // Nombre de coeurs CPU à utiliser
} LLMConfig;

// Initialise le moteur llama.cpp avec le cache KV
bool llm_init(LLMConfig config);

// Génère une reformulation en utilisant le batching
char* llm_generate_paraphrase(const char* input_text);

// Libère la mémoire (très important pour 1Go de modèle)
void llm_cleanup();

#endif