#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <cjson/cJSON.h>

#include "rules/checkers/section_checker.h"

static void test_section_order_success(void **state) {

    const char* text =
        "Résumé\n"
        "Introduction\n"
        "Méthodologie\n"
        "Conclusion";

    cJSON* array = cJSON_CreateArray();

    cJSON_AddItemToArray(array, cJSON_CreateString("Résumé"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Introduction"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Méthodologie"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Conclusion"));

    assert_true(check_section_order(text, array));

    cJSON_Delete(array);
}

static void test_section_order_fail(void **state) {

    const char* text =
        "Conclusion\n"
        "Introduction\n"
        "Résumé";

    cJSON* array = cJSON_CreateArray();

    cJSON_AddItemToArray(array, cJSON_CreateString("Résumé"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Introduction"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Conclusion"));

    assert_false(check_section_order(text, array));

    cJSON_Delete(array);
}

int main(void) {

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_section_order_success),
        cmocka_unit_test(test_section_order_fail),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}