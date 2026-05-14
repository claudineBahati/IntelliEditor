#include <string.h>

int check_contains(const char* text, const char* value) {
    if (!text || !value) return 0;

    return strstr(text, value) != NULL;
}