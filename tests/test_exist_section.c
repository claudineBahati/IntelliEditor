#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include "rules/rule_parser.h"
#include "rules/rule_engine.h"

static void test_rule_engine(void **state) {

    printf("START TEST\n");

    RuleSet* ruleset = load_rules("../data/rule_template/memoire.json");
    assert_non_null(ruleset);

    const char* text =
        "Introduction\n"
        "Ceci est un document de test.\n";

    int result_count = 0;
    RuleResult* results = evaluate_rules(ruleset, text, &result_count);

    assert_non_null(results);
    assert_true(result_count > 0);

    for (int i = 0; i < result_count; i++) {
        printf("Rule: %s -> %s\n",
               results[i].rule_name,
               results[i].success ? "OK" : "FAIL");
    }

    free(results);
    free_ruleset(ruleset);

    printf("END TEST\n");
}

int main(void) {
    
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_rule_engine),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}