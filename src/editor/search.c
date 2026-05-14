#include "search.h"
#include "debug_memory.h"
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

size_t search_replace(GapBuffer* gb, const char* query, const char* replacement, size_t start_pos, bool case_sensitive) {
    if (!gb || !query || !replacement) return (size_t)-1;
    
    size_t query_len = strlen(query);
    if (query_len == 0) return (size_t)-1;
    
    // Trouver la position de la chaîne à remplacer
    size_t pos = search_find(gb, query, start_pos, case_sensitive);
    if (pos == (size_t)-1) return (size_t)-1;
    
    // Sauvegarder la position actuelle du curseur
    size_t original_cursor = gb->gap_start;
    
    // Déplacer le curseur au début de la chaîne trouvée
    gb_move_cursor(gb, (int)(pos - gb->gap_start));
    
    // Supprimer la chaîne query caractère par caractère
    for (size_t i = 0; i < query_len; i++) {
        gb_delete_delete(gb);
    }
    
    // Insérer la chaîne de remplacement
    gb_insert_string(gb, replacement);
    
    // Restaurer la position du curseur à la fin du remplacement
    size_t new_pos = gb->gap_start;
    
    // Remettre le curseur à sa position originale si nécessaire
    // (optionnel, selon la logique souhaitée)
    
    return new_pos;
}

size_t search_replace_all(GapBuffer* gb, const char* query, const char* replacement, bool case_sensitive) {
    if (!gb || !query || !replacement) return 0;
    
    size_t count = 0;
    size_t start_pos = 0;
    size_t query_len = strlen(query);
    size_t replacement_len = strlen(replacement);
    
    while (true) {
        size_t pos = search_find(gb, query, start_pos, case_sensitive);
        if (pos == (size_t)-1) break;
        
        // Effectuer le remplacement
        search_replace(gb, query, replacement, pos, case_sensitive);
        count++;
        
        // Mettre à jour start_pos pour continuer la recherche après le remplacement
        start_pos = pos + replacement_len;
        
        // Sécurité
        if (start_pos >= gb_get_length(gb)) break;
    }
    
    return count;
}
