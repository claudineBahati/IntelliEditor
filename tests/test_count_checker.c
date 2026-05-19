#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "rules/checkers/count_checker.h"


static void test_count_min_success(void **state) {

    const char* text =
        "Introduction\n"
        "Ceci est un texte contenant suffisamment de mots "
        "pour réussir le test correctement.";

    int result = check_min_word_count(
        text,
        "Introduction",
        10
    );

    assert_true(result);
}


static void test_count_min_fail(void **state) {

    const char* text =
        "Introduction\n"
        "Petit texte.";

    int result = check_min_word_count(
        text,
        "Introduction",
        20
    );

    assert_false(result);
}


static void test_word_count_max_success(void **state) {

    const char* text =
        "Conclusion\n"
        "Petit texte simple.";

    int result = check_max_word_count(
        text,
        "Conclusion",
        50
    );

    assert_true(result);
}

static void test_word_count_max_fail(void **state) {

    const char* text =
        "Conclusion\n"
        "Ce texte contient beaucoup beaucoup beaucoup "
        "de mots afin de dépasser la limite maximale.";

    int result = check_max_word_count(
        text,
        "Conclusion",
        5
    );

    assert_false(result);
}


static void test_section_not_found(void **state) {

    const char* text =
        "Bibliographie\n"
        "Aucune introduction.";

    int result = check_min_word_count(
        text,
        "Introduction",
        5
    );

    assert_false(result);
}

int main(void) {

    const struct CMUnitTest tests[] = {

        cmocka_unit_test(test_count_min_success),
        cmocka_unit_test(test_count_min_fail),

        cmocka_unit_test(test_word_count_max_success),
        cmocka_unit_test(test_word_count_max_fail),

        cmocka_unit_test(test_section_not_found),
    };

    return cmocka_run_group_tests(
        tests,
        NULL,
        NULL
    );
}