
#ifndef RULE_PARSER_H
#define RULE_PARSER_H
<<<<<<< HEAD

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
=======

#include <windows.h>
#include "models/rule_set.h"
#include "rules/rule_engine.h"

RuleSet* load_rules(const char* file_path);
void free_ruleset(RuleSet* ruleset);
void apply_rules(HWND hEditor, RuleSet* ruleset);

#endif
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
