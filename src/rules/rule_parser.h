#ifndef RULE_PARSER_H
#define RULE_PARSER_H
#include "models/rule_set.h"

#include <windows.h>

RuleSet* load_rules(const char* file_path);
void free_ruleset(RuleSet* ruleset);
void apply_rules(HWND hEditor, RuleSet* ruleset);

#endif