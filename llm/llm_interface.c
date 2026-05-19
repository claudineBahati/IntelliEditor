#include "llm_interface.h"
#include "llm_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool llm_init(LLMConfig config) {
    printf("[MOCK LLM] Initialization mocked.\n");
    return true;
}

char* llm_generate_paraphrase(const char* input_text) {
    if (input_text == NULL || strlen(input_text) == 0) {
        return strdup("");
    }
    
    char* response = malloc(2048);
    snprintf(response, 2048, "[MOCK LLM Suggestion] %s", input_text);
    return response;
}

void llm_cleanup() {
    printf("[MOCK LLM] Cleanup mocked.\n");
}