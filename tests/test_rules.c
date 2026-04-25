#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "rules/rule_parser.h"

static void test_load_rules(void **state) {
    RuleSet* ruleset = load_rules("data/rules/sample_rules.json");

    assert_non_null(ruleset);
    assert_int_equal(ruleset->count, 5);

    free_ruleset(ruleset);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_load_rules),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}