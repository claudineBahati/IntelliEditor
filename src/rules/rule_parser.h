#ifndef RULE_PARSER_H
#define RULE_PARSER_H
#include "models/rule_set.h"

RuleSet* load_rules(const char* file_path);
void free_ruleset(RuleSet* ruleset);

#endif