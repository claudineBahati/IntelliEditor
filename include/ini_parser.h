#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <stddef.h>

/**
 * Représente une entrée Clé=Valeur associée à une Section.
 */
typedef struct {
    char* section;
    char* key;
    char* value;
} IniEntry;

/**
 * Structure contenant toute la configuration en mémoire.
 */
typedef struct {
    IniEntry* entries;
    size_t count;
    size_t capacity;
} IniConfig;

/**
 * Lit un fichier INI et charge son contenu en mémoire.
 * @param filename Chemin vers le fichier INI.
 * @return Une nouvelle structure IniConfig, ou NULL si le fichier n'existe pas.
 */
IniConfig* ini_load(const char* filename);

/**
 * Récupère une valeur depuis la configuration chargée.
 * @param config La structure chargée via ini_load.
 * @param section Le nom de la section (ex: "Editor").
 * @param key Le nom de la clé (ex: "Theme").
 * @param default_value Valeur de secours retournée si la section/clé n'est pas trouvée.
 * @return La valeur trouvée ou default_value.
 */
const char* ini_get_value(const IniConfig* config, const char* section, const char* key, const char* default_value);

/**
 * Libère toute la mémoire associée à la configuration.
 */
void ini_free(IniConfig* config);

#endif // INI_PARSER_H
