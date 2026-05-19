#include "json_parser.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>

bool parse_llm_response(const char *json_input, char *output_buffer, int buffer_size) {
    if (json_input == NULL) return false;

    // On transforme la chaîne brute en objet JSON
    cJSON *json = cJSON_Parse(json_input);
    if (json == NULL) return false;

    bool found = false;

    // On cherche la clé "choices" qui est une liste
    cJSON *choices = cJSON_GetObjectItem(json, "choices");
    if (cJSON_IsArray(choices) && cJSON_GetArraySize(choices) > 0) {
        
        // On prend le premier élément de la liste
        cJSON *item = cJSON_GetArrayItem(choices, 0);
        cJSON *message = cJSON_GetObjectItem(item, "message");
        cJSON *content = cJSON_GetObjectItem(message, "content");

        if (cJSON_IsString(content) && (content->valuestring != NULL)) {
            // On copie le texte trouvé dans notre buffer de sortie
            strncpy(output_buffer, content->valuestring, buffer_size - 1);
            output_buffer[buffer_size - 1] = '\0';
            found = true;
        }
    }

    // IMPORTANT : Toujours libérer la mémoire de cJSON
    cJSON_Delete(json);
    return found;
}