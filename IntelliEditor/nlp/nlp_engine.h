#ifndef NLP_ENGINE_H
#define NLP_ENGINE_H

#include <stdbool.h>
#include "tokenizer.h"
#include "hunspell_wrap.h"

// 1. Structure pour les erreurs d'orthographe (Hunspell)
typedef struct {
    char* word;
    int word_index;
    char** suggestions; // On ajoute les suggestions d'Hunspell
    int suggest_count;
} SpellingError;

// 2. NOUVEAU : Structure pour les erreurs grammaticales (LLM / Sémantique)
typedef struct {
    char original_phrase[256];
    char suggested_correction[256];
    char error_type[64]; // ex: "Accord sujet-verbe" ou "Majuscule"
} GrammarError;

// Structure pour stocker les propositions de reformulation
 typedef struct {
    char suggestions[3][256]; 
    int suggestion_count;
 } ParaphraseReport;

  // Fonction pour générer des reformulations
  ParaphraseReport nlp_generate_paraphrase(const char* text);

// 3. Rapport complet de correction
typedef struct {
    SpellingError* sp_errors;
    int sp_error_count;
    GrammarError* gr_errors;
    int gr_error_count;
} CorrectionReport;

// Initialisation et Nettoyage
bool nlp_init(const char* aff_path, const char* dic_path);
void nlp_cleanup();

// Le Pipeline complet de la Phase 3
CorrectionReport nlp_process_text(const char* text);
void nlp_free_report(CorrectionReport* report);

#endif