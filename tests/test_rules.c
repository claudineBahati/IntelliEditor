#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "rules/rule_parser.h"
static void test_load_rules(void **state) {
    printf("START TEST\n");

    RuleSet* ruleset = load_rules("data/rule_template/memoire.json");

    printf("AFTER LOAD\n");

    assert_non_null(ruleset);

    printf("RULE COUNT: %d\n", ruleset->count);

    free_ruleset(ruleset);
}


int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_load_rules),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}