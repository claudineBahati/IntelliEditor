#include <string.h>
#include <cjson/cJSON.h>
#include "section_checker.h"

int check_section_exists(const char* text, const char* section) {
    if (!text || !section) return 0;

    return strstr(text, section) != NULL;
}


int check_section_order(
    const char* text,
    cJSON* sections
) {

    if (!text || !sections) {
        return 0;
    }

    if (!cJSON_IsArray(sections)) {
        return 0;
    }

    int previous_position = -1;

    int size = cJSON_GetArraySize(sections);

    for (int i = 0; i < size; i++) {

        cJSON* item = cJSON_GetArrayItem(sections, i);

        if (!cJSON_IsString(item)) {
            return 0;
        }

        char* found = strstr(text, item->valuestring);

        if (!found) {
            return 0;
        }

        int current_position = found - text;

        if (current_position < previous_position) {
            return 0;
        }

        previous_position = current_position;
    }

    return 1;
}