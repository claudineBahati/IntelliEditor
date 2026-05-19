<<<<<<< HEAD
#include <stdio.h>
=======
#include "rule_parser.h"
#include <Scintilla.h>
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
#include <string.h>
#include <stdlib.h>

#include "rule_engine.h"
#include "checkers/section_checker.h"
#include "checkers/count_checker.h"
#include "checkers/regex_checker.h"
#include "checkers/heading_checker.h"
#include "checkers/citation_checker.h"

<<<<<<< HEAD
=======
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
            const char* pattern = NULL;
            if (r->parameter && r->parameter->valuestring) {
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
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

static void add_result(
    RuleReport *report,
    const char *id,
    RuleStatus  status,
    const char *message
) {
    if (report->count >= MAX_RESULTS)
        return;

    RuleResult *r = &report->results[report->count++];

    strncpy(r->rule_id, id,      sizeof(r->rule_id)  - 1);
    strncpy(r->message, message, sizeof(r->message)  - 1);
    r->rule_id[sizeof(r->rule_id) - 1] = '\0';
    r->message[sizeof(r->message) - 1] = '\0';
    r->status = status;

    /* Mise à jour des compteurs de synthèse */
    switch (status) {
        case RULE_OK:      report->ok_count++;      break;
        case RULE_WARNING: report->warning_count++; break;
        case RULE_ERROR:   report->error_count++;   break;
        case RULE_PENDING: report->pending_count++;  break;
    }
}

/* ------------------------------------------------------------------ */
/* Évalue l'ensemble des règles sur le document fourni                 */
/* ------------------------------------------------------------------ */
RuleReport evaluate_rules(RuleSet *ruleset, const char *document) {
    RuleReport report = {0};

    for (int i = 0; i < ruleset->count; i++) {
<<<<<<< HEAD
        Rule *rule = &ruleset->rules[i];

        /* ── section_exists ───────────────────────────────────────── */
        if (strcmp(rule->check_type, "section_exists") == 0) {
            int ok = check_section_exists(document, rule->parameter);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_ERROR,
                       rule->description);
        }

        /* ── section_order ────────────────────────────────────────── */
        else if (strcmp(rule->check_type, "section_order") == 0) {
            int ok = check_section_order(document, rule->parameter);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_WARNING,
                       rule->description);
        }

        /* ── word_count_min ───────────────────────────────────────── */
        else if (strcmp(rule->check_type, "word_count_min") == 0) {
            int min_words = extract_min_words(rule->parameter);
            int ok        = check_word_count_min(document, min_words);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_WARNING,
                       rule->description);
        }

        /* ── word_count_max ───────────────────────────────────────── */
        else if (strcmp(rule->check_type, "word_count_max") == 0) {
            int max_words = extract_max_words(rule->parameter);
            int ok        = check_word_count_max(document, max_words);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_WARNING,
                       rule->description);
=======
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
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
        }

        /* ── regex_forbidden ──────────────────────────────────────── */
        else if (strcmp(rule->check_type, "regex_forbidden") == 0) {
            int found = check_forbidden_pattern(document, rule->parameter,
                                                rule->flags);
            add_result(&report, rule->id,
                       found ? RULE_ERROR : RULE_OK,
                       rule->description);
        }

        /* ── regex_required ───────────────────────────────────────── */
        else if (strcmp(rule->check_type, "regex_required") == 0) {
            int found = check_required_pattern(document, rule->parameter,
                                               rule->flags);
            add_result(&report, rule->id,
                       found ? RULE_OK : RULE_WARNING,
                       rule->description);
        }

        /* ── heading_format ───────────────────────────────────────── */
        else if (strcmp(rule->check_type, "heading_format") == 0) {
            int ok = check_heading_format(document, rule->parameter);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_WARNING,
                       rule->description);
        }

        /* ── citation_present ─────────────────────────────────────── */
        else if (strcmp(rule->check_type, "citation_present") == 0) {
            int ok = check_citation_present(document, rule->parameter);
            add_result(&report, rule->id,
                       ok ? RULE_OK : RULE_ERROR,
                       rule->description);
        }

        /* ── llm_semantic ─────────────────────────────────────────── */
        else if (strcmp(rule->check_type, "llm_semantic") == 0) {
            /*
             * La vérification sémantique LLM est asynchrone.
             * On marque la règle PENDING ; le module LLM (DEV-C)
             * viendra mettre à jour ce résultat via update_rule_result().
             */
            add_result(&report, rule->id,
                       RULE_PENDING,
                       "Vérification sémantique LLM en attente…");
        }

        /* ── type inconnu ─────────────────────────────────────────── */
        else {
            add_result(&report, rule->id,
                       RULE_PENDING,
                       "Type de vérification non implémenté");
        }
    }

<<<<<<< HEAD
    return report;
}


=======
    return results;
}
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
