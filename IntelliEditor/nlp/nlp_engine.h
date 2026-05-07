#ifndef NLP_ENGINE_H
#define NLP_ENGINE_H

#include "tokenizer.h"
#include "hunspell_wrap.h" // On ajoute ça pour faire le lien

typedef struct {
    char* word;
    int word_index;
} SpellingError;

typedef struct {
    SpellingError* errors;
    int error_count;
} CorrectionReport;

// --- AJOUTE CES DEUX LIGNES ICI ---
bool nlp_init(const char* aff_path, const char* dic_path);
void nlp_cleanup();
// ----------------------------------

CorrectionReport nlp_process_text(const char* text);
void nlp_free_report(CorrectionReport* report);

#endif