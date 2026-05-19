#ifndef RULE_ENGINE_H
#define RULE_ENGINE_H

#define MAX_RULES    128
#define MAX_RESULTS  128

/* ------------------------------------------------------------------ */
/* Statuts possibles pour une règle évaluée                            */
/* ------------------------------------------------------------------ */
typedef enum {
    RULE_OK,       /* règle respectée                        */
    RULE_WARNING,  /* règle partiellement respectée          */
    RULE_ERROR,    /* règle clairement violée                */
    RULE_PENDING   /* vérification LLM en attente / inconnue */
} RuleStatus;

/* ------------------------------------------------------------------ */
/* Représente une règle issue du fichier JSON                          */
/* ------------------------------------------------------------------ */
typedef struct {
    char id[32];
    char category[64];
    char severity[32];
    char description[256];
    char check_type[64];
    char parameter[512];
    char flags[128];
    char target_section[128];
} Rule;

/* ------------------------------------------------------------------ */
/* Ensemble de règles chargées depuis un fichier JSON                  */
/* ------------------------------------------------------------------ */
typedef struct {
    Rule rules[MAX_RULES];
    int  count;
} RuleSet;

/* ------------------------------------------------------------------ */
/* Résultat de l'évaluation d'une règle                                */
/* ------------------------------------------------------------------ */
typedef struct {
    char       rule_id[32];
    RuleStatus status;
    char       message[256];
} RuleResult;

/* ------------------------------------------------------------------ */
/* Rapport global : agrégat de tous les résultats                      */
/* ------------------------------------------------------------------ */
typedef struct {
    RuleResult results[MAX_RESULTS];
    int        count;
    int        ok_count;
    int        warning_count;
    int        error_count;
    int        pending_count;
} RuleReport;

/* ------------------------------------------------------------------ */
/* API publique                                                         */
/* ------------------------------------------------------------------ */
RuleReport evaluate_rules(RuleSet *ruleset, const char *document);

#ifdef _WIN32
#include <windows.h>
void apply_rules(HWND hEditor, RuleSet* ruleset);
#endif

#endif /* RULE_ENGINE_H */
