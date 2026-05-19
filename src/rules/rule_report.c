#include <stdio.h>
#include <string.h>

#include "rule_report.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

const char *rule_status_text(RuleStatus status) {
    switch (status) {
        case RULE_OK:      return "[OK]";
        case RULE_WARNING: return "[WARNING]";
        case RULE_ERROR:   return "[ERROR]";
        case RULE_PENDING: return "[PENDING]";
        default:           return "[UNKNOWN]";
    }
}

const char *rule_status_icon(RuleStatus status) {
    switch (status) {
        case RULE_OK:      return "OK     ";
        case RULE_WARNING: return "AVERT. ";
        case RULE_ERROR:   return "ERREUR ";
        case RULE_PENDING: return "ATTENTE";
        default:           return "?      ";
    }
}

int rule_report_conformity_percent(const RuleReport *report) {
    if (report->count == 0) return 0;
    return (report->ok_count * 100) / report->count;
}

/* ------------------------------------------------------------------ */
/* Affichage formaté du rapport                                         */
/* ------------------------------------------------------------------ */

void print_rule_report(const RuleReport *report) {
    printf("\n");
    printf("========================================\n");
    printf("        RAPPORT DE CONFORMITE           \n");
    printf("========================================\n\n");

    for (int i = 0; i < report->count; i++) {
        const RuleResult *res = &report->results[i];

        printf("  %s %s  %s\n",
               rule_status_icon(res->status),
               res->rule_id,
               res->message);
    }

    printf("\n----------------------------------------\n");
    printf("  Total    : %d règle(s)\n", report->count);
    printf("  OK       : %d\n", report->ok_count);
    printf("  Avert.   : %d\n", report->warning_count);
    printf("  Erreurs  : %d\n", report->error_count);
    printf("  En attente: %d\n", report->pending_count);
    printf("  Conformité: %d%%\n",
           rule_report_conformity_percent(report));
    printf("========================================\n\n");
}

/* ------------------------------------------------------------------ */
/* Mise à jour asynchrone d'un résultat (retour LLM)                   */
/* ------------------------------------------------------------------ */

void update_rule_result(
    RuleReport *report,
    const char *rule_id,
    RuleStatus  new_status,
    const char *new_message
) {
    for (int i = 0; i < report->count; i++) {
        RuleResult *res = &report->results[i];
        if (strcmp(res->rule_id, rule_id) != 0) continue;

        /* Décrémente l'ancien compteur */
        switch (res->status) {
            case RULE_OK:      report->ok_count--;      break;
            case RULE_WARNING: report->warning_count--; break;
            case RULE_ERROR:   report->error_count--;   break;
            case RULE_PENDING: report->pending_count--;  break;
        }

        /* Applique la mise à jour */
        res->status = new_status;
        strncpy(res->message, new_message, sizeof(res->message) - 1);
        res->message[sizeof(res->message) - 1] = '\0';

        /* Incrémente le nouveau compteur */
        switch (new_status) {
            case RULE_OK:      report->ok_count++;      break;
            case RULE_WARNING: report->warning_count++; break;
            case RULE_ERROR:   report->error_count++;   break;
            case RULE_PENDING: report->pending_count++;  break;
        }
        return;
    }
    fprintf(stderr,
            "[rule_report] update_rule_result: règle '%s' introuvable\n",
            rule_id);
}
