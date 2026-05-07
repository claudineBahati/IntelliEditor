#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "rules/checkers/regex_checker.h"

static void test_regex_forbidden_fail(void **state) {
    const char* text = "Je suis étudiant";

    assert_false(check_regex_forbidden(text, "\\bje\\b", "case_insensitive"));
}

static void test_regex_forbidden_ok(void **state) {
    const char* text = "Nous sommes étudiants";

    assert_true(check_regex_forbidden(text, "\\bje\\b", "case_insensitive"));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_regex_forbidden_fail),
        cmocka_unit_test(test_regex_forbidden_ok),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}