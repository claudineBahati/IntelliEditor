#include "editor_core.h"
#include "file_io.h"
#include "search.h"
#include <stdlib.h>
#include <string.h>

EditorContext* editor_create(size_t initial_size) {
    EditorContext* ctx = (EditorContext*)calloc(1, sizeof(EditorContext));
    if (!ctx) return NULL;

    ctx->buffer = gb_create(initial_size);
    if (!ctx->buffer) {
        free(ctx);
        return NULL;
    }

    ctx->history = history_create(64);
    if (!ctx->history) {
        gb_destroy(ctx->buffer);
        free(ctx);
        return NULL;
    }

    ctx->file_path = NULL;
    ctx->modified = false;
    return ctx;
}

void editor_destroy(EditorContext* ctx) {
    if (!ctx) return;
    if (ctx->file_path) free(ctx->file_path);
    history_destroy(ctx->history);
    gb_destroy(ctx->buffer);
    free(ctx);
}

bool editor_open_file(EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;
    if (!file_load(filepath, ctx->buffer)) return false;

    if (ctx->file_path) free(ctx->file_path);
    ctx->file_path = strdup(filepath);
    ctx->modified = false;
    return true;
}

bool editor_save_file(EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;
    if (!file_save(filepath, ctx->buffer)) return false;

    if (ctx->file_path) free(ctx->file_path);
    ctx->file_path = strdup(filepath);
    ctx->modified = false;
    return true;
}

char* editor_get_text(const EditorContext* ctx) {
    if (!ctx) return NULL;
    return gb_get_text(ctx->buffer);
}

size_t editor_get_line_count(const EditorContext* ctx) {
    if (!ctx) return 0;
    return gb_get_line_count(ctx->buffer);
}

void editor_get_pos_coords(const EditorContext* ctx, size_t pos, size_t* out_line, size_t* out_col) {
    if (!ctx) return;
    gb_get_pos_coords(ctx->buffer, pos, out_line, out_col);
}

size_t editor_get_line_start(const EditorContext* ctx, size_t line) {
    if (!ctx) return 0;
    return gb_get_line_start(ctx->buffer, line);
}

size_t editor_get_cursor_index(const EditorContext* ctx) {
    if (!ctx || !ctx->buffer) return 0;
    return ctx->buffer->gap_start;
}

const char* editor_get_file_path(const EditorContext* ctx) {
    if (!ctx) return NULL;
    return ctx->file_path;
}

bool editor_is_modified(const EditorContext* ctx) {
    if (!ctx) return false;
    return ctx->modified;
}

bool editor_insert_text(EditorContext* ctx, const char* text) {
    if (!ctx || !text) return false;
    size_t pos = ctx->buffer->gap_start;
    gb_insert_string(ctx->buffer, text);
    history_record(ctx->history, ACTION_INSERT, pos, text);
    ctx->modified = true;
    return true;
}

bool editor_delete_backspace(EditorContext* ctx) {
    if (!ctx || ctx->buffer->gap_start == 0) return false;
    
    size_t pos = ctx->buffer->gap_start - 1;
    char deleted_char[2] = { gb_get_char_at(ctx->buffer, pos), '\0' };
    
    gb_delete_backspace(ctx->buffer);
    history_record(ctx->history, ACTION_DELETE, pos, deleted_char);
    ctx->modified = true;
    return true;
}

bool editor_delete_delete(EditorContext* ctx) {
    if (!ctx) return false;
    size_t pos = ctx->buffer->gap_start;
    char deleted_char[2] = { gb_get_char_at(ctx->buffer, pos), '\0' };
    if (deleted_char[0] == '\0') return false;

    gb_delete_delete(ctx->buffer);
    history_record(ctx->history, ACTION_DELETE, pos, deleted_char);
    ctx->modified = true;
    return true;
}

void editor_move_cursor(EditorContext* ctx, int offset) {
    if (!ctx) return;
    gb_move_cursor(ctx->buffer, offset);
}

bool editor_undo(EditorContext* ctx) {
    const EditorAction* action = history_undo(ctx->history);
    if (!action) return false;

    if (action->type == ACTION_INSERT) {
        // Pour annuler une insertion, on supprime
        size_t current_pos = ctx->buffer->gap_start;
        gb_move_cursor(ctx->buffer, (int)action->position - (int)current_pos);
        for (size_t i = 0; i < strlen(action->text); i++) {
            gb_delete_delete(ctx->buffer);
        }
    } else {
        // Pour annuler une suppression, on ré-insère
        size_t current_pos = ctx->buffer->gap_start;
        gb_move_cursor(ctx->buffer, (int)action->position - (int)current_pos);
        gb_insert_string(ctx->buffer, action->text);
    }
    return true;
}

bool editor_redo(EditorContext* ctx) {
    const EditorAction* action = history_redo(ctx->history);
    if (!action) return false;

    if (action->type == ACTION_INSERT) {
        size_t current_pos = ctx->buffer->gap_start;
        gb_move_cursor(ctx->buffer, (int)action->position - (int)current_pos);
        gb_insert_string(ctx->buffer, action->text);
    } else {
        size_t current_pos = ctx->buffer->gap_start;
        gb_move_cursor(ctx->buffer, (int)action->position - (int)current_pos);
        for (size_t i = 0; i < strlen(action->text); i++) {
            gb_delete_delete(ctx->buffer);
        }
    }
    return true;
}

size_t editor_search(const EditorContext* ctx, const char* query, size_t start_pos, bool case_sensitive) {
    if (!ctx) return (size_t)-1;
    return search_find(ctx->buffer, query, start_pos, case_sensitive);
}

bool editor_set_file_path(EditorContext* ctx, const char* filepath) {
    if (!ctx) return false;
    if (ctx->file_path) free(ctx->file_path);
    ctx->file_path = filepath ? strdup(filepath) : NULL;
    return true;
}
