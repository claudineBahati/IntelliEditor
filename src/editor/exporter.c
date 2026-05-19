#include "exporter.h"
#include "file_io.h"
#include "debug_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool exporter_to_text(const EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;
    return file_save(filepath, ctx->buffer);
}

/* Retourne l'index couleur RTF (1-4) ou 0 pour défaut */
static int get_rtf_color_index(uint32_t color) {
    if (color == 0x0000FF) return 1; /* Bleu  */
    if (color == 0x008000) return 2; /* Vert  */
    if (color == 0xA31515) return 3; /* Rouge */
    if (color == 0x7F0055) return 4; /* Violet*/
    return 0;
}

/* Assure la capacité du buffer de sortie */
static bool ensure_space(char** buf, size_t* cap, size_t pos, size_t needed) {
    if (pos + needed < *cap) return true;
    size_t new_cap = (*cap) * 2 + needed;
    char* nb = (char*)realloc(*buf, new_cap);
    if (!nb) return false;
    *buf = nb;
    *cap = new_cap;
    return true;
}

/* Append une chaîne dans le buffer */
static void buf_append(char** buf, size_t* cap, size_t* pos, const char* s, size_t n) {
    if (!ensure_space(buf, cap, *pos, n + 1)) return;
    memcpy(*buf + *pos, s, n);
    *pos += n;
}

#define BUFA(literal) buf_append(&rtf, &cap, &pos, (literal), sizeof(literal)-1)
#define BUFN(s, n)    buf_append(&rtf, &cap, &pos, (s), (n))

bool exporter_to_rtf(const EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath || !ctx->formatter) return false;

    char* text = editor_get_text(ctx);
    if (!text) return false;

    size_t len = strlen(text);

    /* Allouer un buffer initial genereux */
    size_t cap = len * 12 + 512;
    char*  rtf = (char*)malloc(cap);
    if (!rtf) { free(text); return false; }
    size_t pos = 0;

    /* --- En-tete RTF --- */
    BUFA("{\\rtf1\\ansi\\deff0 {\\fonttbl {\\f0 Courier New;}}\r\n");
    BUFA("{\\colortbl ;");
    BUFA("\\red0\\green0\\blue255;");    /* 1 = bleu     */
    BUFA("\\red0\\green128\\blue0;");    /* 2 = vert     */
    BUFA("\\red163\\green21\\blue21;");  /* 3 = rouge    */
    BUFA("\\red127\\green0\\blue85;");   /* 4 = violet   */
    BUFA("\\red255\\green255\\blue0;");  /* 5 = jaune    */
    BUFA("}\r\n\\f0\\fs24\\cf0\\cb0 ");

    /* --- Corps: regroupement par plage de style identique --- */
    bool cur_has_style = false;
    TextStyle cur_style;
    memset(&cur_style, 0, sizeof(cur_style));

    for (size_t i = 0; i < len; i++) {
        TextStyle new_style;
        memset(&new_style, 0, sizeof(new_style));
        bool new_has_style = formatter_get_style_at(ctx->formatter, i, &new_style);

        bool changed = (new_has_style != cur_has_style) ||
            (new_has_style && (
                new_style.fg_color  != cur_style.fg_color ||
                new_style.bg_color  != cur_style.bg_color ||
                new_style.bold      != cur_style.bold     ||
                new_style.italic    != cur_style.italic   ||
                new_style.underline != cur_style.underline));

        if (changed) {
            /* Fermer l'ancien style */
            if (cur_has_style) {
                if (cur_style.bold)      BUFA("\\b0 ");
                if (cur_style.italic)    BUFA("\\i0 ");
                if (cur_style.underline) BUFA("\\ulnone ");
                if (cur_style.bg_color != 0) BUFA("\\highlight0 ");
            }
            /* Ouvrir le nouveau style */
            if (new_has_style) {
                char tmp[32];
                int n;
                
                // Couleur de texte
                n = snprintf(tmp, sizeof(tmp), "\\cf%d ", get_rtf_color_index(new_style.fg_color));
                BUFN(tmp, (size_t)n);
                
                // Couleur de fond (Highlight)
                if (new_style.bg_color != 0) {
                    // Par défaut on utilise le jaune (5) si une couleur de fond est présente
                    // On pourrait étendre get_rtf_color_index pour le fond aussi
                    int bg_idx = (new_style.bg_color == 0xFFFF00) ? 5 : 5; 
                    n = snprintf(tmp, sizeof(tmp), "\\highlight%d ", bg_idx);
                    BUFN(tmp, (size_t)n);
                }

                if (new_style.bold)      BUFA("\\b ");
                if (new_style.italic)    BUFA("\\i ");
                if (new_style.underline) BUFA("\\ul ");
            } else {
                BUFA("\\cf0\\highlight0\\b0\\i0\\ulnone ");
            }
            cur_style     = new_style;
            cur_has_style = new_has_style;
        }

        /* Ecrire le caractere escape RTF */
        if (!ensure_space(&rtf, &cap, pos, 8)) break;
        char c = text[i];
        if (c == '\n') {
            const char par[] = "\\par\r\n";
            memcpy(rtf + pos, par, 6); pos += 6;
        } else if (c == '\\' || c == '{' || c == '}') {
            rtf[pos++] = '\\';
            rtf[pos++] = c;
        } else if (c == '\t') {
            const char tab[] = "\\tab ";
            memcpy(rtf + pos, tab, 5); pos += 5;
        } else {
            rtf[pos++] = c;
        }
    }

    /* Fermer le dernier style */
    if (cur_has_style) {
        if (cur_style.bold)   BUFA("\\b0 ");
        if (cur_style.italic) BUFA("\\i0 ");
    }

    /* Pied RTF */
    BUFA("}\r\n");
    free(text);

    /* Ecriture en une seule passe */
    FILE* file = fopen(filepath, "wb");
    if (!file) { free(rtf); return false; }
    fwrite(rtf, 1, pos, file);
    fclose(file);
    free(rtf);

    return true;
}

#define IED_MAGIC "IED\0"
#define IED_VERSION 1

bool exporter_to_ied(const EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath || !ctx->formatter) return false;

    FILE* file = fopen(filepath, "wb");
    if (!file) return false;

    // 1. En-tête
    fwrite(IED_MAGIC, 1, 4, file);
    uint32_t version = IED_VERSION;
    fwrite(&version, sizeof(uint32_t), 1, file);

    // 2. Texte
    char* text = editor_get_text(ctx);
    if (!text) { fclose(file); return false; }
    
    uint64_t text_len = (uint64_t)strlen(text);
    fwrite(&text_len, sizeof(uint64_t), 1, file);
    fwrite(text, 1, (size_t)text_len, file);
    free(text);

    // 3. Styles
    uint64_t style_count = (uint64_t)ctx->formatter->count;
    fwrite(&style_count, sizeof(uint64_t), 1, file);
    
    for (size_t i = 0; i < ctx->formatter->count; i++) {
        StyleRange range = ctx->formatter->ranges[i];
        uint64_t start = (uint64_t)range.start;
        uint64_t length = (uint64_t)range.length;
        fwrite(&start, sizeof(uint64_t), 1, file);
        fwrite(&length, sizeof(uint64_t), 1, file);
        fwrite(&range.style, sizeof(TextStyle), 1, file);
    }

    fclose(file);
    return true;
}

bool importer_from_ied(EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;

    FILE* file = fopen(filepath, "rb");
    if (!file) return false;

    // 1. En-tête
    char magic[4];
    if (fread(magic, 1, 4, file) != 4 || memcmp(magic, IED_MAGIC, 4) != 0) {
        fclose(file);
        return false;
    }
    
    uint32_t version;
    if (fread(&version, sizeof(uint32_t), 1, file) != 1 || version != IED_VERSION) {
        fclose(file);
        return false;
    }

    // 2. Texte
    uint64_t text_len;
    if (fread(&text_len, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    
    char* text = (char*)malloc((size_t)text_len + 1);
    if (!text) { fclose(file); return false; }
    
    if (fread(text, 1, (size_t)text_len, file) != text_len) {
        free(text); fclose(file); return false;
    }
    text[text_len] = '\0';

    // Remplacer le contenu de l'éditeur
    gb_destroy(ctx->buffer);
    ctx->buffer = gb_create((size_t)text_len + 32);
    gb_insert_string(ctx->buffer, text);
    free(text);

    // 3. Styles
    formatter_clear_ranges(ctx->formatter);
    uint64_t style_count;
    if (fread(&style_count, sizeof(uint64_t), 1, file) != 1) { fclose(file); return true; }

    for (uint64_t i = 0; i < style_count; i++) {
        uint64_t start, length;
        TextStyle style;
        if (fread(&start, sizeof(uint64_t), 1, file) != 1) break;
        if (fread(&length, sizeof(uint64_t), 1, file) != 1) break;
        if (fread(&style, sizeof(TextStyle), 1, file) != 1) break;
        
        formatter_add_range(ctx->formatter, (size_t)start, (size_t)length, style);
    }

    fclose(file);
    return true;
}
