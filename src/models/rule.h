#ifndef RULE_H
#define RULE_H

typedef struct {
    char* id;
    char* category;
    char* severity;
    char* description;
    char* check_type;
    char* parameter_str;   // pour string
    void* parameter_obj;   // pour objets JSON (plus tard)
    char* flags;
} Rule;

#endif