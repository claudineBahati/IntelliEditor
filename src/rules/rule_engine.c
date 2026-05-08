#include <string.h>
#include <stdlib.h>

#include "rules/rule_engine.h"
#include "checkers/section_checker.h"
#include "checkers/contains_checker.h"
#include "rules/checkers/regex_checker.h"
#include "rules/checkers/count_checker.h"

RuleResult* evaluate_rules(RuleSet* ruleset, const char* text, int* result_count) {
    *result_count = ruleset->count;

    RuleResult* results = malloc(sizeof(RuleResult) * ruleset->count);

    for (int i = 0; i < ruleset->count; i++) {

        Rule rule = ruleset->rules[i];
        results[i].rule_name = rule.description;

        int success = 0;

        
        if (strcmp(rule.check_type, "section_exists") == 0) {
            success = check_section_exists(
                text,
                rule.parameter->valuestring
            );

        } else if (strcmp(rule.check_type, "contains") == 0) {
            success = check_contains(
                text,
                rule.parameter->valuestring
            );

        } 
        else if (strcmp(rule.check_type, "regex_forbidden") == 0) {

            success = check_regex_forbidden(
                text,
                rule.parameter->valuestring,
                rule.flags
            );
        }
        else if (strcmp(rule.check_type, "count_min") == 0) {
            success = check_min_word_count(text, rule.section, rule.min_words);
        }
        else if (strcmp(rule.check_type, "count_max") == 0) {

            success = check_max_word_count(
                text,
                rule.section,
                rule.max_words
            );
        }
        else if (strcmp(rule.check_type, "section_order") == 0) {

            success = check_section_exists(
                text,
                rule.parameter->valuestring
            );
        
        }
        else {
            success = 0;
        }

        results[i].success = success;
        results[i].message = success ? "OK" : "FAILED";
    }

    return results;
}