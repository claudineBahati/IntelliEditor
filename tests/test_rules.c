/*
 * tests/test_rules.c
 * Suite de tests unitaires pour le moteur de règles (DEV-D).
 * Framework : cmocka  (https://cmocka.org)
 *
 * Compilation (exemple MinGW) :
 *   gcc -std=c11 -Wall -I../src/rules -I../src/rules/checkers \
 *       test_rules.c \
 *       ../src/rules/rule_engine.c  ../src/rules/rule_parser.c \
 *       ../src/rules/rule_report.c  \
 *       ../src/rules/checkers/section_checker.c  \
 *       ../src/rules/checkers/count_checker.c    \
 *       ../src/rules/checkers/regex_checker.c    \
 *       ../src/rules/checkers/heading_checker.c  \
 *       ../src/rules/checkers/citation_checker.c \
 *       -lcmocka -o test_rules
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

<<<<<<< HEAD
#include "rule_engine.h"
#include "rule_parser.h"
#include "rule_report.h"
#include "checkers/section_checker.h"
#include "checkers/count_checker.h"
#include "checkers/regex_checker.h"
#include "checkers/heading_checker.h"
#include "checkers/citation_checker.h"
=======
    RuleSet* ruleset = load_rules("data/rule_templates/memoiry_license.json");
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

/* ================================================================== */
/* Données de test                                                      */
/* ================================================================== */

static const char *DOC_COMPLET =
    "# INTRODUCTION\n"
    "Résumé\n"
    "Ce document présente notre recherche sur les systèmes embarqués. "
    "L'Université Don Bosco de Lubumbashi (UDBL) encadre ce travail. "
    "La problématique centrale est : comment optimiser la consommation "
    "énergétique des microcontrôleurs dans des environnements contraints ? "
    "Nous avons structuré ce mémoire en plusieurs parties distinctes afin "
    "de couvrir tous les aspects du sujet de manière exhaustive et rigoureuse.\n\n"
    "Introduction\n"
    "Ce mémoire étudie les techniques d'optimisation.\n\n"
    "Revue de littérature\n"
    "Plusieurs auteurs ont traité ce sujet.\n\n"
    "Méthodologie\n"
    "Nous avons utilisé une approche expérimentale.\n\n"
    "Résultats\n"
    "Les résultats montrent une réduction de 30%.\n\n"
    "Discussion\n"
    "Ces résultats confirment notre hypothèse.\n\n"
    "Conclusion\n"
    "Ce travail ouvre des perspectives intéressantes.\n\n"
    "Bibliographie\n"
    "[1] Dupont, J. (2020). Systèmes embarqués. Paris: Éditions Tech.\n";

static const char *DOC_INCOMPLET =
    "# introduction\n"  /* H1 non en majuscules */
    "Introduction\n"
    "Court texte sans beaucoup de mots.\n"
    "Je pense que ce document est trop court.\n"; /* première personne */

/* ================================================================== */
/* Tests — section_checker                                             */
/* ================================================================== */

static void test_section_exists_found(void **state) {
    (void)state;
    assert_int_equal(check_section_exists(DOC_COMPLET, "Introduction"), 1);
    assert_int_equal(check_section_exists(DOC_COMPLET, "Conclusion"),   1);
    assert_int_equal(check_section_exists(DOC_COMPLET, "Bibliographie"),1);
}

static void test_section_exists_not_found(void **state) {
    (void)state;
    assert_int_equal(check_section_exists(DOC_INCOMPLET, "Conclusion"),   0);
    assert_int_equal(check_section_exists(DOC_INCOMPLET, "Bibliographie"),0);
    assert_int_equal(check_section_exists(DOC_INCOMPLET, "Méthodologie"), 0);
}

static void test_section_exists_case_insensitive(void **state) {
    (void)state;
    /* Le checker doit être insensible à la casse */
    assert_int_equal(check_section_exists(DOC_COMPLET, "introduction"), 1);
    assert_int_equal(check_section_exists(DOC_COMPLET, "CONCLUSION"),   1);
}

static void test_section_exists_null_inputs(void **state) {
    (void)state;
    assert_int_equal(check_section_exists(NULL, "Introduction"), 0);
    assert_int_equal(check_section_exists(DOC_COMPLET, NULL),   0);
    assert_int_equal(check_section_exists(DOC_COMPLET, ""),     0);
}

static void test_section_order_correct(void **state) {
    (void)state;
    const char *order = "Résumé,Introduction,Méthodologie,Conclusion";
    assert_int_equal(check_section_order(DOC_COMPLET, order), 1);
}

static void test_section_order_incorrect(void **state) {
    (void)state;
    /* Dans DOC_COMPLET, Conclusion vient APRÈS Méthodologie */
    const char *order = "Conclusion,Introduction";
    assert_int_equal(check_section_order(DOC_COMPLET, order), 0);
}

static void test_section_order_missing_section(void **state) {
    (void)state;
    /* Une section absente ne doit pas invalider l'ordre */
    const char *order = "Introduction,SectionInexistante,Conclusion";
    assert_int_equal(check_section_order(DOC_COMPLET, order), 1);
}

/* ================================================================== */
/* Tests — count_checker                                               */
/* ================================================================== */

static void test_count_words_basic(void **state) {
    (void)state;
    assert_int_equal(count_words("un deux trois"), 3);
    assert_int_equal(count_words(""), 0);
    assert_int_equal(count_words("   "), 0);
    assert_int_equal(count_words("mot"), 1);
}

static void test_count_words_whitespace(void **state) {
    (void)state;
    assert_int_equal(count_words("  un   deux  trois  "), 3);
    assert_int_equal(count_words("a\tb\nc"), 3);
}

static void test_word_count_min_pass(void **state) {
    (void)state;
    assert_int_equal(check_word_count_min("un deux trois", 3), 1);
    assert_int_equal(check_word_count_min("un deux trois", 2), 1);
}

static void test_word_count_min_fail(void **state) {
    (void)state;
    assert_int_equal(check_word_count_min("un deux", 3), 0);
}

static void test_word_count_max_pass(void **state) {
    (void)state;
    assert_int_equal(check_word_count_max("un deux trois", 3), 1);
    assert_int_equal(check_word_count_max("un deux trois", 10), 1);
}

static void test_word_count_max_fail(void **state) {
    (void)state;
    assert_int_equal(check_word_count_max("un deux trois quatre", 3), 0);
}

static void test_extract_min_words_simple(void **state) {
    (void)state;
    assert_int_equal(extract_min_words("300"), 300);
    assert_int_equal(extract_min_words("0"),   0);
}

static void test_extract_min_words_object(void **state) {
    (void)state;
    const char *param = "{ \"section\": \"Introduction\", \"min_words\": 300 }";
    assert_int_equal(extract_min_words(param), 300);
}

static void test_extract_max_words_object(void **state) {
    (void)state;
    const char *param = "{ \"section\": \"Résumé\", \"max_words\": 250 }";
    assert_int_equal(extract_max_words(param), 250);
}

/* ================================================================== */
/* Tests — regex_checker                                               */
/* ================================================================== */

static void test_forbidden_word_found(void **state) {
    (void)state;
    assert_int_equal(check_forbidden_word("Je pense que oui", "je"), 1);
    assert_int_equal(check_forbidden_word("Bonjour je m'appelle", "bonjour"), 1);
}

static void test_forbidden_word_not_found(void **state) {
    (void)state;
    assert_int_equal(check_forbidden_word("Il pense que oui", "je"), 0);
}

static void test_forbidden_pattern_alternation(void **state) {
    (void)state;
    const char *pattern = "je|me|mon";
    assert_int_equal(check_forbidden_pattern("je fais", pattern, "case_insensitive"), 1);
    assert_int_equal(check_forbidden_pattern("il fait", pattern, "case_insensitive"), 0);
    assert_int_equal(check_forbidden_pattern("Ici mon projet", pattern, "case_insensitive"), 1);
}

static void test_forbidden_pattern_case_sensitive(void **state) {
    (void)state;
    /* Sans flag case_insensitive, "JE" ne doit pas matcher "je" */
    assert_int_equal(check_forbidden_pattern("JE fais", "je", NULL), 0);
}

static void test_required_expression_found(void **state) {
    (void)state;
    assert_int_equal(check_required_expression(DOC_COMPLET, "UDBL"), 1);
}

static void test_required_expression_not_found(void **state) {
    (void)state;
    assert_int_equal(check_required_expression(DOC_INCOMPLET, "UDBL"), 0);
}

/* ================================================================== */
/* Tests — heading_checker                                             */
/* ================================================================== */

static void test_heading_uppercase_pass(void **state) {
    (void)state;
    const char *doc  = "# INTRODUCTION\n## sous-titre\n";
    const char *param = "{ \"level\": 1, \"case\": \"uppercase\" }";
    assert_int_equal(check_heading_format(doc, param), 1);
}

static void test_heading_uppercase_fail(void **state) {
    (void)state;
    const char *doc  = "# introduction\n";
    const char *param = "{ \"level\": 1, \"case\": \"uppercase\" }";
    assert_int_equal(check_heading_format(doc, param), 0);
}

static void test_heading_no_title_neutral(void **state) {
    (void)state;
    /* Aucun H1 → neutre (retourne 1) */
    const char *doc   = "## Sous-section\nTexte.\n";
    const char *param = "{ \"level\": 1, \"case\": \"uppercase\" }";
    assert_int_equal(check_heading_format(doc, param), 1);
}

static void test_heading_capitalize(void **state) {
    (void)state;
    const char *doc_ok   = "## Introduction générale\n";
    const char *doc_fail = "## introduction générale\n";
    const char *param    = "{ \"level\": 2, \"case\": \"capitalize\" }";
    assert_int_equal(check_heading_format(doc_ok,   param), 1);
    assert_int_equal(check_heading_format(doc_fail, param), 0);
}

/* ================================================================== */
/* Tests — citation_checker                                            */
/* ================================================================== */

static void test_citation_found(void **state) {
    (void)state;
    assert_int_equal(check_citation_present(DOC_COMPLET,
        "Bibliographie|Références"), 1);
}

static void test_citation_not_found(void **state) {
    (void)state;
    assert_int_equal(check_citation_present(DOC_INCOMPLET,
        "Bibliographie|Références"), 0);
}

static void test_citation_alternative_match(void **state) {
    (void)state;
    const char *doc = "Texte…\nReferences\nAuteur, 2020.\n";
    assert_int_equal(check_citation_present(doc,
        "Bibliographie|References|Bibliography"), 1);
}

/* ================================================================== */
/* Tests — evaluate_rules (intégration)                               */
/* ================================================================== */

static void test_evaluate_section_exists_ok(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id, "T001", sizeof(r->id)-1);
    strncpy(r->check_type, "section_exists", sizeof(r->check_type)-1);
    strncpy(r->parameter, "Introduction",   sizeof(r->parameter)-1);
    strncpy(r->description, "Test section", sizeof(r->description)-1);

    RuleReport rpt = evaluate_rules(&rs, DOC_COMPLET);
    assert_int_equal(rpt.count, 1);
    assert_int_equal(rpt.results[0].status, RULE_OK);
}

static void test_evaluate_section_exists_error(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id, "T002", sizeof(r->id)-1);
    strncpy(r->check_type, "section_exists", sizeof(r->check_type)-1);
    strncpy(r->parameter, "Bibliographie",  sizeof(r->parameter)-1);
    strncpy(r->description, "Biblio manquante", sizeof(r->description)-1);

    RuleReport rpt = evaluate_rules(&rs, DOC_INCOMPLET);
    assert_int_equal(rpt.results[0].status, RULE_ERROR);
    assert_int_equal(rpt.error_count, 1);
}

static void test_evaluate_regex_forbidden(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id,          "T003",           sizeof(r->id)-1);
    strncpy(r->check_type,  "regex_forbidden", sizeof(r->check_type)-1);
    strncpy(r->parameter,   "je|me|mon",      sizeof(r->parameter)-1);
    strncpy(r->flags,       "case_insensitive",sizeof(r->flags)-1);
    strncpy(r->description, "Pas de 1ère personne", sizeof(r->description)-1);

    /* DOC_INCOMPLET contient "Je" */
    RuleReport rpt = evaluate_rules(&rs, DOC_INCOMPLET);
    assert_int_equal(rpt.results[0].status, RULE_ERROR);
}

static void test_evaluate_word_count_warning(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id,          "T004",           sizeof(r->id)-1);
    strncpy(r->check_type,  "word_count_min", sizeof(r->check_type)-1);
    strncpy(r->parameter,   "1000",           sizeof(r->parameter)-1);
    strncpy(r->description, "Trop court",     sizeof(r->description)-1);

    RuleReport rpt = evaluate_rules(&rs, "Trop court.");
    assert_int_equal(rpt.results[0].status, RULE_WARNING);
}

static void test_evaluate_pending_unknown(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id,          "T005",       sizeof(r->id)-1);
    strncpy(r->check_type,  "llm_semantic",sizeof(r->check_type)-1);
    strncpy(r->description, "Sémantique", sizeof(r->description)-1);

    RuleReport rpt = evaluate_rules(&rs, DOC_COMPLET);
    assert_int_equal(rpt.results[0].status, RULE_PENDING);
    assert_int_equal(rpt.pending_count, 1);
}

static void test_evaluate_counters(void **state) {
    (void)state;
    RuleSet rs = {0};

    /* OK */
    Rule *r0 = &rs.rules[rs.count++];
    strncpy(r0->id, "C001", sizeof(r0->id)-1);
    strncpy(r0->check_type, "section_exists", sizeof(r0->check_type)-1);
    strncpy(r0->parameter, "Introduction", sizeof(r0->parameter)-1);
    strncpy(r0->description, "ok", sizeof(r0->description)-1);

    /* ERROR */
    Rule *r1 = &rs.rules[rs.count++];
    strncpy(r1->id, "C002", sizeof(r1->id)-1);
    strncpy(r1->check_type, "section_exists", sizeof(r1->check_type)-1);
    strncpy(r1->parameter, "Conclusion", sizeof(r1->parameter)-1);
    strncpy(r1->description, "erreur", sizeof(r1->description)-1);

    RuleReport rpt = evaluate_rules(&rs, DOC_INCOMPLET);
    assert_int_equal(rpt.count, 2);
    assert_int_equal(rpt.ok_count, 1);
    assert_int_equal(rpt.error_count, 1);
    assert_int_equal(rule_report_conformity_percent(&rpt), 50);
}

/* ================================================================== */
/* Tests — rule_parser                                                 */
/* ================================================================== */

static void test_load_rules_from_file(void **state) {
    (void)state;
    /* Ce test nécessite que le fichier soit accessible depuis le répertoire
       d'exécution. Adapte le chemin si nécessaire. */
    RuleSet rs = {0};
    int ok = load_rules_from_file(
        "../data/rule_templates/memory_license.json", &rs);

    assert_int_equal(ok, 1);
    assert_true(rs.count > 0);

    /* Vérifie que R001 a bien été parsé */
    int found = 0;
    for (int i = 0; i < rs.count; i++) {
        if (strcmp(rs.rules[i].id, "R001") == 0) {
            found = 1;
            assert_string_equal(rs.rules[i].check_type, "section_exists");
            assert_string_equal(rs.rules[i].parameter, "Introduction");
        }
    }
    assert_int_equal(found, 1);
}

static void test_load_rules_invalid_file(void **state) {
    (void)state;
    RuleSet rs = {0};
    int ok = load_rules_from_file("fichier_inexistant.json", &rs);
    assert_int_equal(ok, 0);
    assert_int_equal(rs.count, 0);
}

/* ================================================================== */
/* Tests — update_rule_result                                          */
/* ================================================================== */

static void test_update_rule_result(void **state) {
    (void)state;
    RuleSet rs = {0};
    Rule *r = &rs.rules[rs.count++];
    strncpy(r->id, "U001", sizeof(r->id)-1);
    strncpy(r->check_type, "llm_semantic", sizeof(r->check_type)-1);
    strncpy(r->description, "Sémantique LLM", sizeof(r->description)-1);

    RuleReport rpt = evaluate_rules(&rs, DOC_COMPLET);
    assert_int_equal(rpt.results[0].status, RULE_PENDING);
    assert_int_equal(rpt.pending_count, 1);

    /* Simule le retour asynchrone du LLM */
    update_rule_result(&rpt, "U001", RULE_OK, "Problématique bien posée");

    assert_int_equal(rpt.results[0].status, RULE_OK);
    assert_int_equal(rpt.ok_count, 1);
    assert_int_equal(rpt.pending_count, 0);
    assert_string_equal(rpt.results[0].message, "Problématique bien posée");
}

/* ================================================================== */
/* Point d'entrée                                                       */
/* ================================================================== */

int main(void) {
    const struct CMUnitTest tests[] = {
        /* section_checker */
        cmocka_unit_test(test_section_exists_found),
        cmocka_unit_test(test_section_exists_not_found),
        cmocka_unit_test(test_section_exists_case_insensitive),
        cmocka_unit_test(test_section_exists_null_inputs),
        cmocka_unit_test(test_section_order_correct),
        cmocka_unit_test(test_section_order_incorrect),
        cmocka_unit_test(test_section_order_missing_section),

        /* count_checker */
        cmocka_unit_test(test_count_words_basic),
        cmocka_unit_test(test_count_words_whitespace),
        cmocka_unit_test(test_word_count_min_pass),
        cmocka_unit_test(test_word_count_min_fail),
        cmocka_unit_test(test_word_count_max_pass),
        cmocka_unit_test(test_word_count_max_fail),
        cmocka_unit_test(test_extract_min_words_simple),
        cmocka_unit_test(test_extract_min_words_object),
        cmocka_unit_test(test_extract_max_words_object),

        /* regex_checker */
        cmocka_unit_test(test_forbidden_word_found),
        cmocka_unit_test(test_forbidden_word_not_found),
        cmocka_unit_test(test_forbidden_pattern_alternation),
        cmocka_unit_test(test_forbidden_pattern_case_sensitive),
        cmocka_unit_test(test_required_expression_found),
        cmocka_unit_test(test_required_expression_not_found),

        /* heading_checker */
        cmocka_unit_test(test_heading_uppercase_pass),
        cmocka_unit_test(test_heading_uppercase_fail),
        cmocka_unit_test(test_heading_no_title_neutral),
        cmocka_unit_test(test_heading_capitalize),

        /* citation_checker */
        cmocka_unit_test(test_citation_found),
        cmocka_unit_test(test_citation_not_found),
        cmocka_unit_test(test_citation_alternative_match),

        /* evaluate_rules (intégration) */
        cmocka_unit_test(test_evaluate_section_exists_ok),
        cmocka_unit_test(test_evaluate_section_exists_error),
        cmocka_unit_test(test_evaluate_regex_forbidden),
        cmocka_unit_test(test_evaluate_word_count_warning),
        cmocka_unit_test(test_evaluate_pending_unknown),
        cmocka_unit_test(test_evaluate_counters),

        /* rule_parser */
        cmocka_unit_test(test_load_rules_from_file),
        cmocka_unit_test(test_load_rules_invalid_file),

        /* update_rule_result */
        cmocka_unit_test(test_update_rule_result),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
