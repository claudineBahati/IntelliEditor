
#ifndef RULE_PARSER_H
#define RULE_PARSER_H

#include "rule_engine.h"

int load_rules_from_file(
    const char* filename,
    RuleSet* ruleset
);

#endif
