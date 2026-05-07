#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdbool.h>

/**
 * @brief Extrait le contenu du message d'une réponse JSON de type LLM.
 * * @param json_input La chaîne de caractères contenant le JSON brut.
 * @param output_buffer Le buffer où copier le texte extrait.
 * @param buffer_size La taille maximale du buffer.
 * @return true si l'extraction a réussi, false sinon.
 */
bool parse_llm_response(const char *json_input, char *output_buffer, int buffer_size);

#endif  