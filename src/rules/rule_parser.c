
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rule_parser.h"

static void extract_value(
    const char* line,
    char* output,
    size_t size
) {

    const char* colon = strchr(line, ':');

    if (!colon)
        return;

    colon++;

    while (*colon == ' ' || *colon == '"')
        colon++;

    size_t i = 0;

    while (*colon &&
           *colon != '"' &&
           *colon != ',' &&
           *colon != '\n' &&
           i < size - 1) {

        output[i++] = *colon++;
    }

    output[i] = '\0';
}

int load_rules_from_file(
    const char* filename,
    RuleSet* ruleset
) {

    FILE* file = fopen(filename, "r");

    if (!file)
        return 0;

    ruleset->count = 0;

    char line[2048];

    Rule current = {0};
    int inside_rule = 0;

    while (fgets(line, sizeof(line), file)) {

        if (strstr(line, "{") && strstr(line, "\"id\"") == NULL) {
            continue;
        }

        if (strstr(line, "\"id\"")) {
            memset(&current, 0, sizeof(Rule));
            inside_rule = 1;
            extract_value(line, current.id, sizeof(current.id));
        }

        if (!inside_rule)
            continue;

        if (strstr(line, "\"category\"")) {
            extract_value(line, current.category, sizeof(current.category));
        }

        if (strstr(line, "\"severity\"")) {
            extract_value(line, current.severity, sizeof(current.severity));
        }

        if (strstr(line, "\"description\"")) {
            extract_value(line, current.description, sizeof(current.description));
        }

        if (strstr(line, "\"check_type\"")) {
            extract_value(line, current.check_type, sizeof(current.check_type));
        }

        if (strstr(line, "\"parameter\"")) {
            extract_value(line, current.parameter, sizeof(current.parameter));
        }

        if (strstr(line, "\"flags\"")) {
            extract_value(line, current.flags, sizeof(current.flags));
        }

        if (strstr(line, "\"target_section\"")) {
            extract_value(line, current.target_section, sizeof(current.target_section));
        }

        if (inside_rule && strstr(line, "}")) {
            ruleset->rules[ruleset->count++] = current;
            inside_rule = 0;
        }
    }

    fclose(file);

    return 1;
}
