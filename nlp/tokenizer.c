#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>

TokenizedText nlp_tokenize(const char* input_text) {
    TokenizedText result;
    result.count = 0;
    result.words = malloc(256 * sizeof(char*)); // On peut stocker jusqu'à 256 mots

    char* text_copy = strdup(input_text);
    // On définit les séparateurs : espace, virgule, points, etc.
    const char* delimiters = " ,.;:!?\n\t\"()";
    
    char* token = strtok(text_copy, delimiters);
    while (token != NULL && result.count < 256) {
        result.words[result.count] = strdup(token);
        result.count++;
        token = strtok(NULL, delimiters);
    }

    free(text_copy);
    return result;
}

void nlp_free_tokens(TokenizedText* tokens) {
    for (int i = 0; i < tokens->count; i++) {
        free(tokens->words[i]);
    }
    free(tokens->words);
}