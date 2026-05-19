#include "llm_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include <windows.h>
#include "llm_prompts.h"

#define MAX_QUEUE 5
#define MAX_TEXT 1024

// --- Structure de la File d'attente ---
typedef struct {
    char items[MAX_QUEUE][MAX_TEXT];
    int head; // Indice de lecture
    int tail; // Indice d'écriture
    int count; // Nombre d'éléments présents
} query_queue_t;

// --- Variables internes ---
static pthread_t worker_thread;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static query_queue_t q = { .head = 0, .tail = 0, .count = 0 };
static bool is_running = false;
static char shared_response[4096];

// --- Fonctions privées de la file ---
static bool queue_push(const char* text) {
    pthread_mutex_lock(&queue_mutex);
    if (q.count >= MAX_QUEUE) {
        pthread_mutex_unlock(&queue_mutex);
        return false; // File pleine
    }
    strncpy(q.items[q.tail], text, MAX_TEXT - 1);
    q.tail = (q.tail + 1) % MAX_QUEUE;
    q.count++;
    pthread_mutex_unlock(&queue_mutex);
    return true;
}

static bool queue_pop(char* dest) {
    pthread_mutex_lock(&queue_mutex);
    if (q.count <= 0) {
        pthread_mutex_unlock(&queue_mutex);
        return false; // File vide
    }
    strncpy(dest, q.items[q.head], MAX_TEXT - 1);
    q.head = (q.head + 1) % MAX_QUEUE;
    q.count--;
    pthread_mutex_unlock(&queue_mutex);
    return true;
}

// --- Le Coeur du Worker (Consommateur) ---
void* llm_worker_func(void* arg) {
    char user_input[MAX_TEXT];
    char final_prompt[MAX_TEXT + 256]; // Buffer plus large pour accueillir le template

    while (is_running) {
        if (queue_pop(user_input)) {
            // --- ÉTAPE : PROMPT ENGINEERING ---
            // On injecte le texte de l'utilisateur dans le template
            snprintf(final_prompt, sizeof(final_prompt), PROMPT_CORRECTION_FORMAT, user_input);
            
            // On affiche le prompt complet pour vérifier le formatage
            printf("\n[Thread] Requête formatée : %s\n", final_prompt);
            
            // Simulation du calcul lourd (ex: inférence LLM)
            Sleep(2000); 
            
            pthread_mutex_lock(&queue_mutex);
            // On simule une réponse basée sur le prompt complet
            snprintf(shared_response, sizeof(shared_response), "Version corrigée de : %s", user_input);
            pthread_mutex_unlock(&queue_mutex);
            
            printf("[Thread] Travail terminé.\n");
        }
        Sleep(50); 
    }
    return NULL;
}


// --- Implémentation API ---
bool llm_thread_init(llm_config_t config) {
    is_running = true;
    return (pthread_create(&worker_thread, NULL, llm_worker_func, NULL) == 0);
}

void llm_thread_query(const char *prompt) {
    if (!queue_push(prompt)) {
        printf("[Erreur] File d'attente pleine !\n");
    }
}

bool llm_thread_is_busy() {
    pthread_mutex_lock(&queue_mutex);
    bool busy = (q.count > 0);
    pthread_mutex_unlock(&queue_mutex);
    return busy;
}

void llm_thread_stop() {
    is_running = false;
    pthread_join(worker_thread, NULL);
    pthread_mutex_destroy(&queue_mutex);
}
// Permet au main de récupérer la réponse simulée
bool llm_thread_get_response(char *buffer, int buffer_size) {
    pthread_mutex_lock(&queue_mutex);
    if (shared_response[0] != '\0') {
        strncpy(buffer, shared_response, buffer_size - 1);
        shared_response[0] = '\0'; // On "vide" la réponse après lecture
        pthread_mutex_unlock(&queue_mutex);
        return true;
    }
    pthread_mutex_unlock(&queue_mutex);
    return false;
}