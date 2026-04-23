#include <stdio.h>
#include "rules/rule_parser.h"

int main() {
    RuleSet* ruleset = load_rules("data/rule_template/memoire.json");

    if (!ruleset) {
        printf("Erreur chargement règles\n");
        return 1;
    }

    for (int i = 0; i < ruleset->count; i++) {
        Rule r = ruleset->rules[i];
        printf("ID: %s\n", r.id);
        printf("Type: %s\n", r.check_type);
        printf("Param: %s\n", r.parameter_str);
        printf("--------\n");
    }

    free_ruleset(ruleset);
    return 0;
}