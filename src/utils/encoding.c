#include "encoding.h"
#include <stdlib.h>

wchar_t* utf8_to_utf16(const char* utf8_str) {
    if (!utf8_str) return NULL;

    // Déterminer la taille requise pour la chaîne UTF-16
    int required_size = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    if (required_size == 0) return NULL;

    // Allouer la mémoire
    wchar_t* utf16_str = (wchar_t*)malloc(required_size * sizeof(wchar_t));
    if (!utf16_str) return NULL;

    // Effectuer la conversion
    if (MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, utf16_str, required_size) == 0) {
        free(utf16_str);
        return NULL;
    }

    return utf16_str;
}

char* utf16_to_utf8(const wchar_t* utf16_str) {
    if (!utf16_str) return NULL;

    // Déterminer la taille requise pour la chaîne UTF-8
    int required_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, NULL, 0, NULL, NULL);
    if (required_size == 0) return NULL;

    // Allouer la mémoire
    char* utf8_str = (char*)malloc(required_size);
    if (!utf8_str) return NULL;

    // Effectuer la conversion
    if (WideCharToMultiByte(CP_UTF8, 0, utf16_str, -1, utf8_str, required_size, NULL, NULL) == 0) {
        free(utf8_str);
        return NULL;
    }

    return utf8_str;
}
