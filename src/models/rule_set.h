#ifndef RULE_SET_H
#define RULE_SET_H

typedef struct {
    char* id;
    char* category;
    char* severity;
    char* description;

    char* check_type;

    char* parameter;      // pour les règles simples
    char* section;        // pour word_count_min
    int min_words;
    int max_words;        // pour word_count_min

    char* flags;          // pour regex
} Rule;

typedef struct {
    Rule* rules;
    int count;
} RuleSet;

#endif