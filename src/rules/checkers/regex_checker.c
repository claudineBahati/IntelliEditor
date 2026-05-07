#define PCRE2_CODE_UNIT_WIDTH 8

#include <string.h>
#include <pcre2.h>

#include "rules/checkers/regex_checker.h"

int check_regex_forbidden(
    const char* text,
    const char* pattern,
    const char* flags
) {

    if (!text || !pattern)
        return 1;

    uint32_t options = 0;

    // case_insensitive
    if (flags && strstr(flags, "case_insensitive")) {
        options |= PCRE2_CASELESS;
    }

    int errornumber;
    PCRE2_SIZE erroroffset;

    pcre2_code* re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        options,
        &errornumber,
        &erroroffset,
        NULL
    );

    // erreur compilation regex
    if (!re) {
        return 1;
    }

    pcre2_match_data* match_data =
        pcre2_match_data_create_from_pattern(re, NULL);

    int rc = pcre2_match(
        re,
        (PCRE2_SPTR)text,
        strlen(text),
        0,
        0,
        match_data,
        NULL
    );

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    // si regex trouvée => FAIL
    if (rc >= 0) {
        return 0;
    }

    return 1;
}