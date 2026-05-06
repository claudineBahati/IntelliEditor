#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <string.h>

int check_regex_forbidden(const char* text, const char* pattern, const char* flags) {

    if (!text || !pattern) return 1;

    int errornumber;
    PCRE2_SIZE erroroffset;

    uint32_t options = 0;

    //  gestion flags
    if (flags && strstr(flags, "case_insensitive")) {
        options |= PCRE2_CASELESS;
    }

    // compilation regex
    pcre2_code *re = pcre2_compile(
        (PCRE2_SPTR)pattern,
        PCRE2_ZERO_TERMINATED,
        options,
        &errornumber,
        &erroroffset,
        NULL
    );

    if (re == NULL) {
        return 1; // ignore si regex invalide
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);

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

    
    if (rc >= 0) {
        return 0; 
    }

    return 1; 
}