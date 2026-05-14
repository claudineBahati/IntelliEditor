#ifndef EDITOR_CORE_H
#define EDITOR_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include "gap_buffer.h"
#include "history.h"
#include "formatter.h"

typedef struct {
    GapBuffer* buffer;
    History* history;
    Formatter* formatter;
    char* file_path;
    bool modified;
} EditorContext;

EditorContext* editor_create(size_t initial_size);
void editor_destroy(EditorContext* ctx);

bool editor_open_file(EditorContext* ctx, const char* filepath);
bool editor_save_file(EditorContext* ctx, const char* filepath);

char* editor_get_text(const EditorContext* ctx);
size_t editor_get_line_count(const EditorContext* ctx);
void editor_get_pos_coords(const EditorContext* ctx, size_t pos, size_t* out_line, size_t* out_col);
size_t editor_get_line_start(const EditorContext* ctx, size_t line);
size_t editor_get_cursor_index(const EditorContext* ctx);
const char* editor_get_file_path(const EditorContext* ctx);
bool editor_is_modified(const EditorContext* ctx);

bool editor_insert_text(EditorContext* ctx, const char* text);
bool editor_delete_backspace(EditorContext* ctx);
bool editor_delete_delete(EditorContext* ctx);
void editor_move_cursor(EditorContext* ctx, int offset);
bool editor_undo(EditorContext* ctx);
bool editor_redo(EditorContext* ctx);

size_t editor_search(const EditorContext* ctx, const char* query, size_t start_pos, bool case_sensitive);
bool editor_replace(EditorContext* ctx, const char* old_text, const char* new_text, bool all, bool case_sensitive);
bool editor_replace_all(EditorContext* ctx, const char* old_text, const char* new_text, bool case_sensitive);
bool editor_set_file_path(EditorContext* ctx, const char* filepath);

#endif // EDITOR_CORE_H
