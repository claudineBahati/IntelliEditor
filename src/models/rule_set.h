#ifndef RULE_SET_H
#define RULE_SET_H

#include "rule.h"

typedef struct {
    Rule* rules;
    int count;
} RuleSet;

#endif