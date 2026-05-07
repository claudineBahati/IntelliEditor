#include "nlp_engine.h"
#include "hunspell_wrap.h"
#include <stdlib.h>
#include <string.h>

CorrectionReport nlp_process_text(const char* text) {
    CorrectionReport report;
    report.error_count = 0;
    report.errors = malloc(100 * sizeof(SpellingError));

    TokenizedText tokens = nlp_tokenize(text);

    for (int i = 0; i < tokens.count; i++) {
        if (!nlp_check_word(tokens.words[i])) {
            report.errors[report.error_count].word = strdup(tokens.words[i]);
            report.errors[report.error_count].word_index = i;
            report.error_count++;
        }
    }

    nlp_free_tokens(&tokens);
    return report;
}

void nlp_free_report(CorrectionReport* report) {
    for (int i = 0; i < report->error_count; i++) {
        free(report->errors[i].word);
    }
    free(report->errors);
}