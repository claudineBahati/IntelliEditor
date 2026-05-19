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

#ifdef _WIN32
#include <windows.h>
#include <Scintilla.h>

void apply_rules(HWND hEditor, RuleSet* ruleset) {
    if (!hEditor || !ruleset) return;

    int length = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
    SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hEditor, SCI_INDICATORCLEARRANGE, 0, length);

    SendMessage(hEditor, SCI_INDICSETSTYLE, 0, INDIC_SQUIGGLE);
    SendMessage(hEditor, SCI_INDICSETFORE, 0, RGB(255, 0, 0));

    for (int i = 0; i < ruleset->count; i++) {
        Rule* r = &ruleset->rules[i];
        
        if (strcmp(r->check_type, "regex_forbidden") == 0) {
            const char* pattern = r->parameter;
            if (!pattern || strlen(pattern) == 0) continue;

            int searchFlags = SCFIND_REGEXP;
            int startPos = 0;
            while (startPos < length) {
                SendMessage(hEditor, SCI_SETTARGETSTART, startPos, 0);
                SendMessage(hEditor, SCI_SETTARGETEND, length, 0);
                SendMessage(hEditor, SCI_SETSEARCHFLAGS, searchFlags, 0);
                
                int foundPos = (int)SendMessage(hEditor, SCI_SEARCHINTARGET, strlen(pattern), (LPARAM)pattern);
                if (foundPos == -1) break;

                int endPos = (int)SendMessage(hEditor, SCI_GETTARGETEND, 0, 0);
                
                SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
                SendMessage(hEditor, SCI_INDICATORFILLRANGE, foundPos, endPos - foundPos);

                startPos = endPos;
            }
        }
    }
}
#endif


