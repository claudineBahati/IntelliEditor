#include "nlp_engine.h"
#include "spellcheck.h"
#include "llm_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool nlp_init(const char* aff_path, const char* dic_path) {
    // Initialiser notre correcteur orthographique local avec le dictionnaire français
    printf("[NLP] Initialisation du correcteur avec dictionnaire_fr.txt...\n");
    return spellcheck_init("dictionnaire_fr.txt");
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
        const char* word = tokens.words[i];
        if (strlen(word) == 0) continue;

        // Éliminer les nombres et caractères non-alphabétiques
        bool has_letter = false;
        for (int k = 0; word[k]; k++) {
            if (isalpha((unsigned char)word[k]) || (unsigned char)word[k] >= 128) {
                has_letter = true;
                break;
            }
        }
        if (!has_letter) continue;

        if (!spellcheck_is_correct(word)) {
            SpellingError* temp = realloc(report.sp_errors, (report.sp_error_count + 1) * sizeof(SpellingError));
            if (temp) {
                report.sp_errors = temp;
                int current = report.sp_error_count;
                
                report.sp_errors[current].word = strdup(word);
                report.sp_errors[current].word_index = i;

                // Récupérer les suggestions réelles de correction
                SpellSuggestions suggestions = spellcheck_get_suggestions(word);
                report.sp_errors[current].suggest_count = suggestions.count;
                
                if (suggestions.count > 0) {
                    report.sp_errors[current].suggestions = malloc(suggestions.count * sizeof(char*));
                    for (int s = 0; s < suggestions.count; s++) {
                        report.sp_errors[current].suggestions[s] = strdup(suggestions.words[s]);
                    }
                } else {
                    report.sp_errors[current].suggestions = NULL;
                }
                
                report.sp_error_count++;
            }
        }
    }

    check_semantic_rules(text, &report);
    call_llm_grammar(text, &report);

    nlp_free_tokens(&tokens);
    return report;
}

void nlp_cleanup() {
    spellcheck_cleanup();
}

void nlp_free_report(CorrectionReport* report) {
    if (report->sp_errors) {
        for (int i = 0; i < report->sp_error_count; i++) {
            free(report->sp_errors[i].word);
            if (report->sp_errors[i].suggestions) {
                for (int s = 0; s < report->sp_errors[i].suggest_count; s++) {
                    free(report->sp_errors[i].suggestions[s]);
                }
                free(report->sp_errors[i].suggestions);
            }
        }
        free(report->sp_errors);
        report->sp_errors = NULL;
    }
    if (report->gr_errors) {
        free(report->gr_errors);
        report->gr_errors = NULL;
    }
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