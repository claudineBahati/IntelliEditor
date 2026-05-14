#include "rule_parser.h"
#include <Scintilla.h>
#include <string.h>
#include <stdlib.h>

#include "rules/rule_engine.h"
#include "checkers/section_checker.h"
#include "checkers/contains_checker.h"
#include "rules/checkers/regex_checker.h"
#include "rules/checkers/count_checker.h"

void apply_rules(HWND hEditor, RuleSet* ruleset) {
    if (!hEditor || !ruleset) return;

    // 1. Nettoyer les anciens indicateurs (on utilise l'indicateur 0 par défaut)
    int length = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
    SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hEditor, SCI_INDICATORCLEARRANGE, 0, length);

    // Configurer l'apparence de l'indicateur (ex: souligné vague rouge)
    SendMessage(hEditor, SCI_INDICSETSTYLE, 0, INDIC_SQUIGGLE);
    SendMessage(hEditor, SCI_INDICSETFORE, 0, RGB(255, 0, 0));

    // 2. Parcourir les règles
    for (int i = 0; i < ruleset->count; i++) {
        Rule* r = &ruleset->rules[i];
        
        if (strcmp(r->check_type, "forbidden_word") == 0 || strcmp(r->check_type, "regex") == 0 || strcmp(r->check_type, "regex_forbidden") == 0) {
            const char* pattern = r->parameter_str;
            if (!pattern && r->parameter && r->parameter->valuestring) {
                pattern = r->parameter->valuestring;
            }
            if (!pattern) continue;

            int searchFlags = SCFIND_NONE;
            if (strcmp(r->check_type, "regex") == 0 || strcmp(r->check_type, "regex_forbidden") == 0) {
                searchFlags |= SCFIND_REGEXP;
            }

            // Recherche et marquage
            int startPos = 0;
            while (startPos < length) {
                SendMessage(hEditor, SCI_SETTARGETSTART, startPos, 0);
                SendMessage(hEditor, SCI_SETTARGETEND, length, 0);
                SendMessage(hEditor, SCI_SETSEARCHFLAGS, searchFlags, 0);
                
                int foundPos = (int)SendMessage(hEditor, SCI_SEARCHINTARGET, strlen(pattern), (LPARAM)pattern);
                if (foundPos == -1) break;

                int endPos = (int)SendMessage(hEditor, SCI_GETTARGETEND, 0, 0);
                
                // Appliquer l'indicateur
                SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
                SendMessage(hEditor, SCI_INDICATORFILLRANGE, foundPos, endPos - foundPos);

                startPos = endPos;
            }
        }
    }
}

RuleResult* evaluate_rules(RuleSet* ruleset, const char* text, int* result_count) {
    if (!ruleset || !text) {
        *result_count = 0;
        return NULL;
    }
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
        else if (strcmp(rule.check_type, "regex_required") == 0) {
            success = check_regex_required(
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
