#include <ctype.h>
#include <string.h>

#include "rules/checkers/count_checker.h"

static int word_count_words(const char* text) {
    int count = 0;
    int in_word = 0;

    for (const char* p = text; *p; p++) {
        if (isspace((unsigned char)*p)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
    }

    return count;
}

int check_min_word_count(const char* text, const char* section, int min_words) {
    if (!text || !section) return 0;

    const char* start = strstr(text, section);
    if (!start) return 0;

   
    start += strlen(section);

    int words = word_count_words(start);
    return words >= min_words;
}

int check_max_word_count(const char* text, const char* section, int max_words) {

    if (!text || !section)
        return 0;

    const char* start = strstr(text, section);

    if (!start)
        return 0;

    start += strlen(section);

    int words = word_count_words(start);

    return words <= max_words;
}