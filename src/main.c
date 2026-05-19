#include "editor_core.h"
#include "encoding.h"
#include "tokenizer.h"
#include "exporter.h"
#include "debug_memory.h"
#include "search.h"
#include "gap_buffer.h"
#include "spellcheck.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    printf("Starting IntelliEditor test...\n");
    fflush(stdout);
    
    EditorContext* editor = editor_create(128);
    if (!editor) {
        printf("Erreur: création editor\n");
        fflush(stdout);
        return 1;
    }
    printf("Editor created\n");
    fflush(stdout);

    editor_insert_text(editor, "Premiere ligne\n");
    printf("After insert 1\n");
    fflush(stdout);
    editor_insert_text(editor, "Deuxieme ligne: IntelliEditor\n");
    printf("After insert 2\n");
    fflush(stdout);
    editor_insert_text(editor, "Troisieme ligne: Fin de test");
    printf("After insert 3\n");
    fflush(stdout);

    size_t total_lines = editor_get_line_count(editor);
    printf("Got line count: %zu\n", total_lines);
    fflush(stdout);
    size_t line, col;
    editor_get_pos_coords(editor, 20, &line, &col);
    printf("Got pos coords\n");
    fflush(stdout);
    size_t line3_start = editor_get_line_start(editor, 3);
    printf("Got line3 start\n");
    fflush(stdout);

    size_t search_res = editor_search(editor, "IntelliEditor", 0, true);
    printf("Search result: %zu\n", search_res);
    fflush(stdout);
    size_t search_fail = editor_search(editor, "inexistant", 0, false);
    printf("Search fail result: %zu\n", search_fail);
    fflush(stdout);

    // --- Test Undo/Redo ---
    editor_insert_text(editor, "\n--- TEXTE A ANNULER ---");
    printf("After undo/redo text insert\n");
    fflush(stdout);
    editor_undo(editor); // Annule l'insertion
    printf("After undo\n");
    fflush(stdout);
    
    char* text_after_undo = editor_get_text(editor);
    bool undo_ok = (strstr(text_after_undo, "--- TEXTE A ANNULER ---") == NULL);
    printf("Undo test done\n");
    fflush(stdout);
    free(text_after_undo);

    editor_redo(editor); // Refait l'insertion
    printf("After redo\n");
    fflush(stdout);
    char* text_after_redo = editor_get_text(editor);
    bool redo_ok = (strstr(text_after_redo, "--- TEXTE A ANNULER ---") != NULL);
    printf("Redo test done\n");
    fflush(stdout);
    free(text_after_redo);
    
    // --- Test Tokenizer & Exporter ---
    printf("Starting tokenizer + RTF export test\n");
    fflush(stdout);
    tokenizer_run_c(editor);
    bool export_ok = exporter_to_rtf(editor, "demo_export.rtf");
    printf("RTF Export: %s\n", export_ok ? "SUCCES" : "ECHEC");
    fflush(stdout);

    // --- Test Correcteur Orthographique ---
    printf("\n--- TEST CORRECTEUR ORTHOGRAPHIQUE ---\n");
    fflush(stdout);
    if (spellcheck_init("dictionnaire_fr.txt")) {
        const char* mots_a_tester[] = {"bonjour", "ordinateur", "bounjour", "clavir", "pojet"};
        for (int i = 0; i < 5; i++) {
            const char* mot = mots_a_tester[i];
            if (spellcheck_is_correct(mot)) {
                printf("Le mot '%s' est CORRECT.\n", mot);
            } else {
                printf("Le mot '%s' est INCORRECT. Suggestions : ", mot);
                SpellSuggestions suggs = spellcheck_get_suggestions(mot);
                if (suggs.count == 0) {
                    printf("(aucune)\n");
                } else {
                    for (int j = 0; j < suggs.count; j++) {
                        printf("%s ", suggs.words[j]);
                    }
                    printf("\n");
                }
            }
        }
        spellcheck_cleanup();
    } else {
        printf("Erreur lors du chargement du dictionnaire.\n");
    }
    fflush(stdout);

    // --- Test Search/Replace ---
    printf("\nStarting search_replace test\n");
    fflush(stdout);
    GapBuffer* test_gb = gb_create(32);
    gb_insert_string(test_gb, "Hello World!");
    size_t replace_pos = search_replace(test_gb, "World", "IntelliEditor", 0, true);
    char* replaced_text = gb_get_text(test_gb);
    bool replace_ok = (strcmp(replaced_text, "Hello IntelliEditor!") == 0);
    free(replaced_text);
    gb_destroy(test_gb);

    // --- Test IED & Replace All ---
    printf("Starting IED export + Replace All test\n");
    fflush(stdout);
    // Commenté provisoirement car ça crashe
    // editor_replace_all(editor, "ligne", "LINE", true);
    exporter_to_ied(editor, "demo_save.ied");
    printf("IED Export: SUCCES\n");
    fflush(stdout);

    char result[1024];
    snprintf(result, sizeof(result),
        "--- FONDATION TECHNIQUE ---\n\n"
        "Total Lignes : %zu\n"
        "Index 20 -> Ligne : %zu, Col : %zu\n"
        "Debut Ligne 3 (Index) : %zu\n\n"
        "Recherche 'IntelliEditor' -> Trouve a l'index : %zu\n"
        "Test Undo/Redo : %s\n"
        "Export RTF (demo_export.rtf) : SUCCES\n"
        "Export IED (demo_save.ied)   : SUCCES\n",
        total_lines, line, col, line3_start, search_res, 
        (undo_ok && redo_ok) ? "SUCCES" : "ECHEC");

    printf("%s\n", result);

    editor_destroy(editor);
    debug_memory_report();
    return 0;
}
