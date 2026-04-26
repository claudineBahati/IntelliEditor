#include <windows.h>
#include "encoding.h"
#include "gap_buffer.h"
#include "ini_parser.h"
#include <stdlib.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Tester le Parser INI
    IniConfig* config = ini_load("config.ini");
    const char* theme = ini_get_value(config, "Editor", "Theme", "DefaultTheme");
    const char* model = ini_get_value(config, "AI", "Model", "NoModel");
    
    char config_msg[256];
    snprintf(config_msg, sizeof(config_msg), "Theme chargé : %s\nModèle chargé : %s", theme, model);
    
    wchar_t* utf16_msg = utf8_to_utf16(config_msg);
    if (utf16_msg) {
        MessageBoxW(NULL, utf16_msg, L"Test Phase 1 - Parser INI", MB_OK | MB_ICONINFORMATION);
        free(utf16_msg);
    }
    ini_free(config);

    // 2. Initialiser le Gap Buffer
    GapBuffer* gb = gb_create(10);
    
    // 2. Simuler une saisie : "Bonjour"
    gb_insert_string(gb, "Bonjour");
    
    // 3. Déplacer le curseur à la fin et ajouter " à tous"
    gb_insert_string(gb, " à tous");
    
    // 4. Déplacer le curseur vers la gauche et insérer un mot au milieu
    // On recule de 8 octets (" à tous" = espace(1) + à(2) + espace(1) + t(1) + o(1) + u(1) + s(1) = 8 octets)
    gb_move_cursor(gb, -8);
    gb_insert_string(gb, " le monde et");
    
    // 5. Récupérer le texte final : "Bonjour le monde et à tous"
    char* text_utf8 = gb_get_text(gb);
    
    // Convertir en UTF-16 pour l'affichage Win32
    wchar_t* utf16_text = utf8_to_utf16(text_utf8);
    
    if (utf16_text) {
        MessageBoxW(NULL, utf16_text, L"Test Phase 1 - Gap Buffer", MB_OK | MB_ICONINFORMATION);
        free(utf16_text);
    } else {
        MessageBoxA(NULL, "Erreur d'encodage.", "Erreur", MB_OK | MB_ICONERROR);
    }
    
    // Nettoyage de la mémoire
    free(text_utf8);
    gb_destroy(gb);
    
    return 0;
}
