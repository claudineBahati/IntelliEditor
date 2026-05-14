#ifndef REGEX_CHECKER_H
#define REGEX_CHECKER_H

int check_regex_forbidden(
    const char* text,
    const char* pattern,
    const char* flags
);
int check_regex_required(
    const char* text,
    const char* pattern,
    const char* flags
);

#endif