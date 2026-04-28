#include <windows.h>
#include "encoding.h"
#include "gap_buffer.h"
#include "search.h"
#include <stdlib.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Initialiser le Gap Buffer
    GapBuffer* gb = gb_create(128);
    
    // 2. Insérer du texte avec des sauts de ligne
    gb_insert_string(gb, "Premiere ligne\n");
    gb_insert_string(gb, "Deuxieme ligne: IntelliEditor\n");
    gb_insert_string(gb, "Troisieme ligne: Fin de test");

    // 3. Test Gestion des Lignes
    size_t total_lines = gb_get_line_count(gb);
    
    size_t line, col;
    // On cherche les coordonnées de l'index 20 (devrait être dans la ligne 2)
    gb_get_pos_coords(gb, 20, &line, &col);
    
    size_t line3_start = gb_get_line_start(gb, 3);

    // 4. Test Moteur de Recherche
    size_t search_res = search_find(gb, "IntelliEditor", 0, true);
    size_t search_fail = search_find(gb, "inexistant", 0, false);

    // 5. Formatage du résultat pour affichage
    char result[1024];
    snprintf(result, sizeof(result), 
        "--- INFRASTRUCTURE TEST (PHASE 3) ---\n\n"
        "Total Lignes : %zu\n"
        "Index 20 -> Ligne : %zu, Col : %zu\n"
        "Debut Ligne 3 (Index) : %zu\n\n"
        "Recherche 'IntelliEditor' -> Trouve a l'index : %zu\n"
        "Recherche 'inexistant' -> Code retour : %zu (NOT_FOUND)",
        total_lines, line, col, line3_start, search_res, search_fail);

    // Affichage Win32
    wchar_t* utf16_text = utf8_to_utf16(result);
    if (utf16_text) {
        MessageBoxW(NULL, utf16_text, L"Dev A - Infrastructure Logic", MB_OK | MB_ICONINFORMATION);
        free(utf16_text);
    }
    
    // Nettoyage
    gb_destroy(gb);
    
    return 0;
}
