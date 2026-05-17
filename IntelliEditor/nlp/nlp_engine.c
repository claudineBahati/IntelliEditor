#include "nlp_engine.h"
#include "llm_interface.h" 
#include <hunspell/hunspell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static Hunhandle* hb = NULL;

bool nlp_init(const char* aff_path, const char* dic_path) {
    hb = Hunspell_create(aff_path, dic_path);
    return (hb != NULL);
}

void check_semantic_rules(const char* text, CorrectionReport* report) {
    if (text[0] != '\0' && islower((unsigned char)text[0])) {
        int idx = report->gr_error_count;
        GrammarError* temp = realloc(report->gr_errors, (idx + 1) * sizeof(GrammarError));
        if (temp) {
            report->gr_errors = temp;
            strcpy(report->gr_errors[idx].original_phrase, "Début de phrase");
            snprintf(report->gr_errors[idx].suggested_correction, 256, "%c...", toupper((unsigned char)text[0]));
            strcpy(report->gr_errors[idx].error_type, "SÉMANTIQUE (Majuscule)");
            report->gr_error_count++;
        }
    }
}

// --- MODIFICATION : APPEL RÉEL LLM ---
void call_llm_grammar(const char* text, CorrectionReport* report) {
    printf("[LLM Engine] Analyse de la structure avec Cache KV...\n");
    
    // On appelle la fonction de génération réelle
    char* ai_fix = llm_generate_paraphrase(text);
    
    if (ai_fix) {
        printf("[LLM Suggestion] : %s\n", ai_fix);
        free(ai_fix); // On libère la mémoire allouée par le moteur
    }
}

CorrectionReport nlp_process_text(const char* text) {
    CorrectionReport report = { .sp_errors = NULL, .sp_error_count = 0, .gr_errors = NULL, .gr_error_count = 0 };
    
    TokenizedText tokens = nlp_tokenize(text);

    for (int i = 0; i < tokens.count; i++) {
        if (hb && Hunspell_spell(hb, tokens.words[i]) == 0) {
            SpellingError* temp = realloc(report.sp_errors, (report.sp_error_count + 1) * sizeof(SpellingError));
            if (temp) {
                report.sp_errors = temp;
                int current = report.sp_error_count;
                
                report.sp_errors[current].word = strdup(tokens.words[i]);
                report.sp_errors[current].word_index = i;

                char** slist;
                int n_suggest = Hunspell_suggest(hb, &slist, tokens.words[i]);
                
                if (n_suggest > 0) {
                    report.sp_error_count++;
                    Hunspell_free_list(hb, &slist, n_suggest);
                } else {
                    report.sp_error_count++;
                }
            }
        }
    }

    check_semantic_rules(text, &report);
    call_llm_grammar(text, &report);

    nlp_free_tokens(&tokens);
    return report;
}

void nlp_cleanup() {
    if (hb) {
        Hunspell_destroy(hb);
        hb = NULL;
    }
}

void nlp_free_report(CorrectionReport* report) {
    if (report->sp_errors) {
        for (int i = 0; i < report->sp_error_count; i++) {
            free(report->sp_errors[i].word);
        }
        free(report->sp_errors);
    }
    if (report->gr_errors) free(report->gr_errors);
    report->sp_error_count = 0;
    report->gr_error_count = 0;
}

// --- MODIFICATION : GÉNÉRATION RÉELLE ---
ParaphraseReport nlp_generate_paraphrase(const char* text) {
    ParaphraseReport report;
    report.suggestion_count = 0;

    if (strlen(text) > 0) {
        // APPEL AU MOTEUR IA (Llama.cpp)
        char* ai_suggestion = llm_generate_paraphrase(text);

        if (ai_suggestion) {
            // Suggestion 1 : Récupérée de l'IA
            strncpy(report.suggestions[0], ai_suggestion, 255);
            report.suggestions[0][255] = '\0';
            
            // Suggestion 2 : Variante simple (pour l'exemple)
            snprintf(report.suggestions[1], 256, "Alternative: %s", ai_suggestion);
            
            report.suggestion_count = 2;
            free(ai_suggestion);
        }
    }

    return report;
}