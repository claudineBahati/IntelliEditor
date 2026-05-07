#ifndef ENCODING_H
#define ENCODING_H

#include <windows.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Convertit une chaîne UTF-8 en une chaîne UTF-16 (wchar_t*).
 * L'appelant est responsable de libérer la mémoire retournée avec free().
 *
 * @param utf8_str La chaîne UTF-8 source.
 * @return Un pointeur vers la nouvelle chaîne UTF-16, ou NULL en cas d'erreur.
 */
wchar_t* utf8_to_utf16(const char* utf8_str);

/**
 * Convertit une chaîne UTF-16 (wchar_t*) en une chaîne UTF-8.
 * L'appelant est responsable de libérer la mémoire retournée avec free().
 *
 * @param utf16_str La chaîne UTF-16 source.
 * @return Un pointeur vers la nouvelle chaîne UTF-8, ou NULL en cas d'erreur.
 */
char* utf16_to_utf8(const wchar_t* utf16_str);

#endif // ENCODING_H
