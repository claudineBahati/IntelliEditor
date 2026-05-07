#include <windows.h>
#include "editor_core.h"
#include "encoding.h"
#include "tokenizer.h"
#include "exporter.h"
#include "debug_memory.h"
#include <stdlib.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    EditorContext* editor = editor_create(128);
    if (!editor) return 1;

    editor_insert_text(editor, "Premiere ligne\n");
    editor_insert_text(editor, "Deuxieme ligne: IntelliEditor\n");
    editor_insert_text(editor, "Troisieme ligne: Fin de test");

    size_t total_lines = editor_get_line_count(editor);
    size_t line, col;
    editor_get_pos_coords(editor, 20, &line, &col);
    size_t line3_start = editor_get_line_start(editor, 3);

    size_t search_res = editor_search(editor, "IntelliEditor", 0, true);
    size_t search_fail = editor_search(editor, "inexistant", 0, false);

    // --- Test Undo/Redo ---
    editor_insert_text(editor, "\n--- TEXTE A ANNULER ---");
    editor_undo(editor); // Annule l'insertion
    
    char* text_after_undo = editor_get_text(editor);
    bool undo_ok = (strstr(text_after_undo, "--- TEXTE A ANNULER ---") == NULL);
    free(text_after_undo);

    editor_redo(editor); // Refait l'insertion
    char* text_after_redo = editor_get_text(editor);
    bool redo_ok = (strstr(text_after_redo, "--- TEXTE A ANNULER ---") != NULL);
    free(text_after_redo);
    
    // --- Test Tokenizer & Exporter ---
    tokenizer_run_c(editor);
    bool export_ok = exporter_to_rtf(editor, "demo_export.rtf");

    char result[1024];
    snprintf(result, sizeof(result),
        "--- FONDATION TECHNIQUE ---\n\n"
        "Total Lignes : %zu\n"
        "Index 20 -> Ligne : %zu, Col : %zu\n"
        "Debut Ligne 3 (Index) : %zu\n\n"
        "Recherche 'IntelliEditor' -> Trouve a l'index : %zu\n"
        "Test Undo/Redo : %s\n"
        "Export RTF (demo_export.rtf) : %s",
        total_lines, line, col, line3_start, search_res, 
        (undo_ok && redo_ok) ? "SUCCES" : "ECHEC",
        export_ok ? "SUCCES" : "ECHEC");

    wchar_t* utf16_text = utf8_to_utf16(result);
    if (utf16_text) {
        MessageBoxW(NULL, utf16_text, L"IntelliEditor - Validation Finale", MB_OK | MB_ICONINFORMATION);
        free(utf16_text);
    }

    editor_destroy(editor);
    return 0;
}
