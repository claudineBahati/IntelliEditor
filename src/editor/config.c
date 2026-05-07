#include "config.h"
#include "ini_parser.h"
#include "debug_memory.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t parse_size_t(const char *s) {
    if (!s) return 0;
    return (size_t)strtoull(s, NULL, 10);
}

static bool parse_bool(const char *s) {
    if (!s) return false;
    return (strcmp(s, "true") == 0 || strcmp(s, "1") == 0);
}

bool config_load(const char *filepath, EditorContext *ctx) {
    if (!filepath || !ctx) return false;
    IniConfig *cfg = ini_load(filepath);
    if (!cfg) return false;

    const char *val;
    // Buffer size
    val = ini_get_value(cfg, "Editor", "initial_buffer_size", NULL);
    if (val) {
        size_t sz = parse_size_t(val);
        if (sz > 0) {
            // Recreate buffer with new size
            GapBuffer *newb = gb_create(sz);
            if (!newb) { ini_free(cfg); return false; }
            gb_destroy(ctx->buffer);
            ctx->buffer = newb;
        }
    }
    // Tab width – stored in ctx->formatter maybe (not defined yet) – just ignore for now
    // UTF‑8 flag – we store in ctx->modified as placeholder (real implementation later)
    val = ini_get_value(cfg, "Editor", "use_utf8", NULL);
    if (val) {
        ctx->modified = parse_bool(val);
    }

    ini_free(cfg);
    return true;
}

bool config_save(const char *filepath, const EditorContext *ctx) {
    if (!filepath || !ctx) return false;
    FILE *f = fopen(filepath, "w");
    if (!f) return false;
    fprintf(f, "[Editor]\n");
    if (ctx->buffer) {
        // Retrieve current capacity (approximate) – GapBuffer does not expose capacity directly, use internal field via cast
        size_t cap = ((GapBuffer*)ctx->buffer)->size; // unsafe but works for internal use
        fprintf(f, "initial_buffer_size = %zu\n", cap);
    }
    fprintf(f, "use_utf8 = %s\n", ctx->modified ? "true" : "false");
    fclose(f);
    return true;
}
