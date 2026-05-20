#ifndef RULE_PARSER_H
#define RULE_PARSER_H

#include "rule_engine.h"

/*
 * Charge les règles depuis un fichier JSON dans *ruleset*.
 * Retourne 1 en cas de succès, 0 en cas d'erreur (fichier introuvable,
 * format invalide, dépassement de MAX_RULES).
 *
 * Le format attendu est décrit dans la section 6 du cahier des charges
 * (champ "rules" : tableau d'objets avec les clés id, category,
 * severity, description, check_type, parameter, flags, target_section).
 */
int load_rules_from_file(const char *filename, RuleSet *ruleset);

/*
 * Affiche sur stdout le contenu du RuleSet (debug).
 */
void print_ruleset(const RuleSet *ruleset);

#endif /* RULE_PARSER_H */