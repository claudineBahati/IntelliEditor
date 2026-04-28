#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool file_load(const char* filepath, GapBuffer* gb) {
    if (!filepath || !gb) return false;

    // Ouverture en mode binaire pour préserver les retours à la ligne natifs
    FILE* file = fopen(filepath, "rb");
    if (!file) return false;

    // Déterminer la taille du fichier
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fclose(file);
        return false;
    }

    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return false;
    }

    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';
    fclose(file);

    // Vider le gap buffer existant en remettant le gap à sa taille maximale
    gb->gap_start = 0;
    gb->gap_end = gb->size;

    // Insérer tout le contenu lu
    gb_insert_string(gb, content);

    free(content);
    return true;
}

bool file_save(const char* filepath, const GapBuffer* gb) {
    if (!filepath || !gb) return false;

    FILE* file = fopen(filepath, "wb");
    if (!file) return false;

    // On récupère la chaîne contiguë du buffer
    char* text = gb_get_text(gb);
    if (text) {
        size_t len = strlen(text);
        fwrite(text, 1, len, file);
        free(text);
    }

    fclose(file);
    return true;
}
