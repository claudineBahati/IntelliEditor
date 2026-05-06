#include <string.h>
#include <stdlib.h>

int check_min_words(const char* text, const char* value) {

    int min = atoi(value);
    int count = 0;

    const char* ptr = text;

    while (*ptr) {
        if (*ptr == ' ' || *ptr == '\n') count++;
        ptr++;
    }

    return count >= min;
}