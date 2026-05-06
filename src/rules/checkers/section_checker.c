#include <string.h>

int check_section_exists(const char* text, const char* section) {
    if (!text || !section) return 0;

    return strstr(text, section) != NULL;
}