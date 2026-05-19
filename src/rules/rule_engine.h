#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#include "models/rule_set.h"  // pour Rule et RuleSet

//  Résultat d'une règle
typedef struct {
    const char* rule_name;
    int success;          // 1 = OK, 0 = FAIL
    const char* message;  // "OK" ou "FAILED"
} RuleResult;

//  Fonction principale du moteur
RuleResult* evaluate_rules(RuleSet* ruleset, const char* text, int* result_count);

#endif