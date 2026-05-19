#ifndef RULE_REPORT_H
#define RULE_REPORT_H

#include "rule_engine.h"

/*
 * Affiche le rapport de conformité sur stdout.
 * Format :
 *   ✅ [OK]      R001 → description
 *   ⚠️ [WARNING] R003 → description
 *   ❌ [ERROR]   R004 → description
 *   🔄 [PENDING] R009 → description
 * suivi d'un résumé chiffré.
 */
void print_rule_report(const RuleReport *report);

/*
 * Retourne le texte de statut court : "[OK]", "[WARNING]", etc.
 */
const char *rule_status_text(RuleStatus status);

/*
 * Retourne l'icône Unicode associée au statut.
 */
const char *rule_status_icon(RuleStatus status);

/*
 * Calcule le pourcentage de conformité (OK / total).
 * Retourne une valeur entre 0 et 100.
 */
int rule_report_conformity_percent(const RuleReport *report);

/*
 * Met à jour un résultat existant dans le rapport (utilisé par DEV-C
 * pour les résultats LLM asynchrones).
 */
void update_rule_result(RuleReport *report,
                        const char *rule_id,
                        RuleStatus  new_status,
                        const char *new_message);

#endif /* RULE_REPORT_H */
