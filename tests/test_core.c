#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gap_buffer.h"
#include "search.h"
#include "encoding.h"
#include "file_io.h"
#include "ini_parser.h"
#include "editor_core.h"

static void test_gap_buffer(void) {
    printf("Running test_gap_buffer...\n");
    fflush(stdout);

    GapBuffer* gb = gb_create(16);
    assert(gb);

    gb_insert_string(gb, "Hello\nWorld");
    char* text = gb_get_text(gb);
    assert(text && strcmp(text, "Hello\nWorld") == 0);
    free(text);

    assert(gb_get_line_count(gb) == 2);

    size_t line, col;
    gb_get_pos_coords(gb, 7, &line, &col);
    assert(line == 2);
    assert(col == 2);
    assert(gb_get_line_start(gb, 2) == 6);

    gb_destroy(gb);
}

static void test_search(void) {
    GapBuffer* gb = gb_create(16);
    assert(gb);
    gb_insert_string(gb, "IntelliEditor\nintellieditor\n");

    size_t idx1 = search_find(gb, "IntelliEditor", 0, true);
    assert(idx1 == 0);

    size_t idx2 = search_find(gb, "intellieditor", 0, false);
    assert(idx2 == 0);

    size_t idx3 = search_find(gb, "inexistant", 0, false);
    assert(idx3 == (size_t)-1);

    gb_destroy(gb);
}

static void test_encoding(void) {
    const char* utf8 = "Hello \xC3\xA9 UTF-8";
    wchar_t* utf16 = utf8_to_utf16(utf8);
    assert(utf16);

    char* utf8_back = utf16_to_utf8(utf16);
    assert(utf8_back);
    assert(strcmp(utf8, utf8_back) == 0);

    free(utf16);
    free(utf8_back);
}

static void test_file_io(void) {
    const char* path = "test_tmp.txt";
    GapBuffer* gb = gb_create(16);
    assert(gb);
    gb_insert_string(gb, "File IO test\nLine 2\n");

    assert(file_save(path, gb));

    GapBuffer* gb2 = gb_create(16);
    assert(gb2);
    assert(file_load(path, gb2));

    char* loaded_text = gb_get_text(gb2);
    assert(loaded_text && strcmp(loaded_text, "File IO test\nLine 2\n") == 0);
    free(loaded_text);

    gb_destroy(gb);
    gb_destroy(gb2);
    remove(path);
}

static void test_ini_parser(void) {
    const char* path = "test_config.ini";
    FILE* file = fopen(path, "w");
    assert(file);
    fputs("[General]\nname=IntelliEditor\nversion=1.0\n", file);
    fclose(file);

    IniConfig* config = ini_load(path);
    assert(config);
    assert(strcmp(ini_get_value(config, "General", "name", ""), "IntelliEditor") == 0);
    assert(strcmp(ini_get_value(config, "General", "version", ""), "1.0") == 0);

    ini_free(config);
    remove(path);
}

static void test_editor_undo_redo(void) {
    printf("Running test_editor_undo_redo...\n");
    fflush(stdout);

    EditorContext* ctx = editor_create(16);
    assert(ctx);

    editor_insert_text(ctx, "Hello");
    char* text1 = editor_get_text(ctx);
    assert(strcmp(text1, "Hello") == 0);
    free(text1);

    editor_insert_text(ctx, " World");
    char* text2 = editor_get_text(ctx);
    assert(strcmp(text2, "Hello World") == 0);
    free(text2);

    // Test Undo
    editor_undo(ctx);
    char* text3 = editor_get_text(ctx);
    assert(strcmp(text3, "Hello") == 0);
    free(text3);

    // Test Redo
    editor_redo(ctx);
    char* text4 = editor_get_text(ctx);
    assert(strcmp(text4, "Hello World") == 0);
    free(text4);

    // Test Delete and Undo
    editor_delete_backspace(ctx);
    char* text5 = editor_get_text(ctx);
    assert(strcmp(text5, "Hello Worl") == 0);
    free(text5);

    editor_undo(ctx);
    char* text6 = editor_get_text(ctx);
    assert(strcmp(text6, "Hello World") == 0);
    free(text6);

    editor_destroy(ctx);
}

int main(void) {
    test_gap_buffer();
    printf("test_gap_buffer OK\n");
    fflush(stdout);

    test_search();
    printf("test_search OK\n");
    fflush(stdout);

    test_encoding();
    printf("test_encoding OK\n");
    fflush(stdout);

    test_file_io();
    printf("test_file_io OK\n");
    fflush(stdout);

    test_ini_parser();
    printf("test_ini_parser OK\n");
    fflush(stdout);

    test_editor_undo_redo();
    printf("test_editor_undo_redo OK\n");
    fflush(stdout);

    printf("All foundation tests passed.\n");
    fflush(stdout);
    return 0;
}
