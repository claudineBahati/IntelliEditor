#include "llm_interface.h"
#include "../llama.cpp/include/llama.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct llama_model * model = NULL;
static struct llama_context * ctx = NULL;

bool llm_init(LLMConfig config) {
    llama_backend_init();
    struct llama_model_params model_params = llama_model_default_params();
    model = llama_model_load_from_file(config.model_path, model_params);
    if (!model) return false;

    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx   = config.n_ctx;
    ctx_params.n_batch = config.n_batch;
    ctx_params.n_threads = config.n_threads;

    ctx = llama_init_from_model(model, ctx_params);
    return ctx != NULL;
}

char* llm_generate_paraphrase(const char* input_text) {
    if (ctx == NULL) {
        fprintf(stderr, "[DEBUG] Erreur : Tentative de generation sans IA chargee !\n");
        return strdup("IA non disponible");
    }

    const struct llama_model * model_ptr = llama_get_model(ctx);
    if (model_ptr == NULL) return strdup("Erreur : Modele introuvable.");

    const struct llama_vocab * vocab_ptr = llama_model_get_vocab(model_ptr);
    if (vocab_ptr == NULL) return strdup("Erreur : Vocabulaire absent.");

    if (input_text == NULL || strlen(input_text) == 0) {
        return strdup("");
    }

    // --- CORRECTION DU PROMPT POUR LE FRANÇAIS ---
    // En structurant le prompt avec "Alternatives :\n-", on force l'IA à aller 
    // droit au but sous forme de tirets, limitant le bavardage inutile.
    // Nouveau prompt avec exemple pour forcer l'IA à corriger les gros mots tronqués
    char prompt[1024];
    snprintf(prompt, sizeof(prompt), 
             "Consigne: Corrige les fautes et donne 3 reformulations en bon francais.\n"
             "Exemple: Le pogramme de cette inteligence est incroiable.\n"
             "Alternatives:\n"
             "- Le programme de cette intelligence est incroyable.\n"
             "- Le logiciel de cette intelligence artificielle est impressionnant.\n"
             "- La technologie de cette IA est vraiment formidable.\n\n"
             "Texte a corriger: %s\n"
             "Alternatives:\n-", 
             input_text);

             
    // --- ÉTAPE A : Tokenization ---
    int n_tokens_max = strlen(prompt) + 4; 
    llama_token * tokens = malloc(n_tokens_max * sizeof(llama_token));
    if (!tokens) return strdup("Erreur d'allocation memoire (tokens)");

    int n_tokens = llama_tokenize(vocab_ptr, prompt, strlen(prompt), tokens, n_tokens_max, true, true);
    if (n_tokens < 0) {
        free(tokens);
        return strdup("Erreur lors de la tokenization");
    }

    // --- OPTIMISATION 2 : Batch LLM ---
    struct llama_batch batch = llama_batch_init(n_tokens, 0, 1);

    for (int i = 0; i < n_tokens; i++) {
        batch.token[i]    = tokens[i];
        batch.pos[i]      = i;         
        batch.n_seq_id[i] = 1;         
        batch.seq_id[i][0] = 0;        
        batch.logits[i]   = (i == n_tokens - 1); 
    }
    batch.n_tokens = n_tokens;

    if (llama_decode(ctx, batch) != 0) {
        free(tokens);
        llama_batch_free(batch);
        return strdup("Erreur : Echec du decodage initial");
    }

    // Initialisation du Sampler moderne 
    struct llama_sampler * smpl = llama_sampler_init_greedy();

    // --- ÉTAPE B : Boucle de génération ---
    char * response_text = malloc(2048);
    response_text[0] = '\0';
    int response_len = 0;

    // CORRECTION DE LA LIMITE : Augmentée à 128 pour laisser l'IA finir sa phrase sans couper au milieu
    int max_generate = 128; 
    for (int i = 0; i < max_generate; i++) {
        llama_token curr_token = llama_sampler_sample(smpl, ctx, batch.n_tokens - 1);

        if (curr_token == llama_vocab_eos(vocab_ptr)) {
            break;
        }

        char piece[128];
        int piece_len = llama_token_to_piece(vocab_ptr, curr_token, piece, sizeof(piece), 0, true);
        if (piece_len > 0 && response_len + piece_len < 2047) {
            strncat(response_text, piece, piece_len);
            response_len += piece_len;
        }

        // --- OPTIMISATION Cache KV continue ---
        batch.n_tokens = 1;
        batch.token[0] = curr_token;
        batch.pos[0]   = n_tokens + i; 
        batch.logits[0] = true;

        if (llama_decode(ctx, batch) != 0) {
            break;
        }
    }

    // --- ÉTAPE C : Nettoyage final de la mémoire ---
    llama_sampler_free(smpl);
    free(tokens);
    llama_batch_free(batch);

    return response_text;
}

void llm_cleanup() {
    if (ctx) llama_free(ctx);
    if (model) llama_model_free(model);
    llama_backend_free();
}