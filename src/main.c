#include <windows.h>
#include "encoding.h"
#include "gap_buffer.h"
#include "ini_parser.h"
#include "file_io.h"
#include "history.h"
#include <stdlib.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Initialiser le Gap Buffer et l'Historique
    GapBuffer* gb = gb_create(10);
    History* hist = history_create(16);
    
    // 2. Test File I/O : Charger "test.txt"
    if (file_load("test.txt", gb)) {
        // Enregistrer l'action dans l'historique
        history_record(hist, ACTION_INSERT, gb->gap_start, " => Fichier chargé !");
        
        // Simuler la frappe
        gb_insert_string(gb, " => Fichier chargé !");
    } else {
        gb_insert_string(gb, "Fichier test.txt non trouvé.");
    }

    // 3. Test Undo
    const EditorAction* undo_action = history_undo(hist);
    char final_msg[512];
    
    char* text_from_gb = gb_get_text(gb);
    if (undo_action && undo_action->type == ACTION_INSERT) {
        // On annule l'insertion (on affiche juste qu'on l'a détecté pour le test)
        snprintf(final_msg, sizeof(final_msg), "Texte actuel : %s\n\nUndo Action interceptée :\nType: INSERT\nTexte à retirer: %s", text_from_gb, undo_action->text);
    } else {
        snprintf(final_msg, sizeof(final_msg), "Texte actuel : %s\n\nAucun Undo possible.", text_from_gb);
    }
    
    // Sauvegarder dans un nouveau fichier pour valider file_save
    file_save("test2.txt", gb);
    
    // Affichage Win32
    wchar_t* utf16_text = utf8_to_utf16(final_msg);
    if (utf16_text) {
        MessageBoxW(NULL, utf16_text, L"Test Phase 2 - File I/O & Undo", MB_OK | MB_ICONINFORMATION);
        free(utf16_text);
    }
    
    // Nettoyage
    free(text_from_gb);
    history_destroy(hist);
    gb_destroy(gb);
    
    return 0;
}
