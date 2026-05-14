#ifndef SECTION_CHECKER_H
#define SECTION_CHECKER_H

#include <cjson/cJSON.h>

int check_section_exists(
    const char* text,
    const char* section
);


int check_section_order(
    const char* text,
    cJSON* sections
);



#endif