#include "search.h"
#include <string.h>
#include <ctype.h>

size_t search_find(const GapBuffer* gb, const char* query, size_t start_pos, bool case_sensitive) {
    if (!gb || !query) return (size_t)-1;
    
    size_t query_len = strlen(query);
    if (query_len == 0) return (size_t)-1;
    
    size_t total_len = gb_get_length(gb);
    
    // Si la position de départ est déjà hors limites
    if (start_pos + query_len > total_len) return (size_t)-1;
    
    for (size_t i = start_pos; i <= total_len - query_len; i++) {
        bool found = true;
        for (size_t j = 0; j < query_len; j++) {
            char b_char = gb_get_char_at(gb, i + j);
            char q_char = query[j];
            
            if (case_sensitive) {
                if (b_char != q_char) {
                    found = false;
                    break;
                }
            } else {
                if (tolower((unsigned char)b_char) != tolower((unsigned char)q_char)) {
                    found = false;
                    break;
                }
            }
        }
        
        if (found) return i;
    }
    
    return (size_t)-1;
}
