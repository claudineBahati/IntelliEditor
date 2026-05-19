#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rule_engine.h"
#include "checkers/section_checker.h"
#include "checkers/count_checker.h"
#include "checkers/regex_checker.h"
#include "checkers/heading_checker.h"
#include "checkers/citation_checker.h"


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

    return report;
}


