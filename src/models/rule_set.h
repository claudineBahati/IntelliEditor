#ifndef RULE_SET_H
#define RULE_SET_H

typedef struct {
    char* id;
    char* category;
    char* severity;
    char* description;

    char* check_type;  
    char* parameter; 
    char* flags;   

} Rule;

typedef struct {
    Rule* rules;
    int count;
} RuleSet;

#endif