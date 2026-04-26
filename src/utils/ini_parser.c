#include "ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper pour enlever les espaces au début et à la fin d'une chaîne
static char* trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static void add_entry(IniConfig* config, const char* section, const char* key, const char* value) {
    if (config->count >= config->capacity) {
        config->capacity = config->capacity == 0 ? 16 : config->capacity * 2;
        IniEntry* new_entries = (IniEntry*)realloc(config->entries, config->capacity * sizeof(IniEntry));
        if (!new_entries) return;
        config->entries = new_entries;
    }
    
    config->entries[config->count].section = section ? strdup(section) : strdup("");
    config->entries[config->count].key = strdup(key);
    config->entries[config->count].value = strdup(value);
    config->count++;
}

IniConfig* ini_load(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    IniConfig* config = (IniConfig*)calloc(1, sizeof(IniConfig));
    if (!config) {
        fclose(file);
        return NULL;
    }

    char line[256];
    char current_section[128] = "";

    while (fgets(line, sizeof(line), file)) {
        char* trimmed_line = trim(line);
        
        // Ignorer lignes vides et commentaires
        if (trimmed_line[0] == '\0' || trimmed_line[0] == ';' || trimmed_line[0] == '#') {
            continue;
        }

        // Section [Name]
        if (trimmed_line[0] == '[' && trimmed_line[strlen(trimmed_line) - 1] == ']') {
            trimmed_line[strlen(trimmed_line) - 1] = '\0'; // Enlever le ']'
            strncpy(current_section, trimmed_line + 1, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
            continue;
        }

        // Clé=Valeur
        char* sep = strchr(trimmed_line, '=');
        if (sep) {
            *sep = '\0';
            char* key = trim(trimmed_line);
            char* value = trim(sep + 1);
            add_entry(config, current_section, key, value);
        }
    }

    fclose(file);
    return config;
}

const char* ini_get_value(const IniConfig* config, const char* section, const char* key, const char* default_value) {
    if (!config || !section || !key) return default_value;

    for (size_t i = 0; i < config->count; i++) {
        if (strcmp(config->entries[i].section, section) == 0 &&
            strcmp(config->entries[i].key, key) == 0) {
            return config->entries[i].value;
        }
    }
    return default_value;
}

void ini_free(IniConfig* config) {
    if (config) {
        for (size_t i = 0; i < config->count; i++) {
            free(config->entries[i].section);
            free(config->entries[i].key);
            free(config->entries[i].value);
        }
        free(config->entries);
        free(config);
    }
}
