#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "rule_parser.h"

// 🔹 Lire fichier entier
static char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur ouverture fichier\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);
    return buffer;
}

// 🔹 Parser une règle
static Rule parse_rule(cJSON* json_rule) {
    Rule rule = {0};

    cJSON* id = cJSON_GetObjectItem(json_rule, "id");
    cJSON* category = cJSON_GetObjectItem(json_rule, "category");
    cJSON* severity = cJSON_GetObjectItem(json_rule, "severity");
    cJSON* description = cJSON_GetObjectItem(json_rule, "description");
    cJSON* check_type = cJSON_GetObjectItem(json_rule, "check_type");
    cJSON* parameter = cJSON_GetObjectItem(json_rule, "parameter");
    cJSON* flags = cJSON_GetObjectItem(json_rule, "flags");

    if (id) rule.id = strdup(id->valuestring);
    if (category) rule.category = strdup(category->valuestring);
    if (severity) rule.severity = strdup(severity->valuestring);
    if (description) rule.description = strdup(description->valuestring);
    if (check_type) rule.check_type = strdup(check_type->valuestring);

    // 🔹 paramètre simple (string)
    if (parameter && cJSON_IsString(parameter)) {
        rule.parameter_str = strdup(parameter->valuestring);
    }

    // 🔹 flags optionnel
    if (flags && cJSON_IsString(flags)) {
        rule.flags = strdup(flags->valuestring);
    }

    return rule;
}

// 🔹 Fonction principale
RuleSet* load_rules(const char* file_path) {
    char* json_data = read_file(file_path);
    if (!json_data) return NULL;

    cJSON* root = cJSON_Parse(json_data);
    if (!root) {
        printf("Erreur parsing JSON\n");
        free(json_data);
        return NULL;
    }

    cJSON* rules_json = cJSON_GetObjectItem(root, "rules");
    if (!cJSON_IsArray(rules_json)) {
        printf("Format rules invalide\n");
        cJSON_Delete(root);
        free(json_data);
        return NULL;
    }

    int count = cJSON_GetArraySize(rules_json);

    RuleSet* ruleset = (RuleSet*)malloc(sizeof(RuleSet));
    ruleset->rules = (Rule*)malloc(sizeof(Rule) * count);
    ruleset->count = count;

    for (int i = 0; i < count; i++) {
        cJSON* json_rule = cJSON_GetArrayItem(rules_json, i);
        ruleset->rules[i] = parse_rule(json_rule);
    }

    cJSON_Delete(root);
    free(json_data);

    return ruleset;
}

// 🔹 Libération mémoire
void free_ruleset(RuleSet* ruleset) {
    for (int i = 0; i < ruleset->count; i++) {
        Rule r = ruleset->rules[i];

        free(r.id);
        free(r.category);
        free(r.severity);
        free(r.description);
        free(r.check_type);
        free(r.parameter_str);
        free(r.flags);
    }

    free(ruleset->rules);
    free(ruleset);
}