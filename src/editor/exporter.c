#include "exporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool exporter_to_text(const EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;
    return editor_save_file((EditorContext*)ctx, filepath);
}

// Fonction utilitaire pour extraire les composantes d'une couleur 0x00RRGGBB
static void get_rgb(uint32_t color, int* r, int* g, int* b) {
    *r = (color >> 16) & 0xFF;
    *g = (color >> 8) & 0xFF;
    *b = color & 0xFF;
}

bool exporter_to_rtf(const EditorContext* ctx, const char* filepath) {
    if (!ctx || !filepath) return false;
    
    FILE* file = fopen(filepath, "w");
    if (!file) return false;
    
    char* text = editor_get_text(ctx);
    if (!text) {
        fclose(file);
        return false;
    }
    
    // 1. En-tête RTF et table des polices
    fprintf(file, "{\\rtf1\\ansi\\deff0 {\\fonttbl {\\f0 Courier New;}}\n");
    
    // 2. Table des couleurs
    fprintf(file, "{\\colortbl ;\\red0\\green0\\blue255;\\red0\\green128\\blue0;\\red163\\green21\\blue21;\\red127\\green0\\blue85;}\n");
    
    fprintf(file, "\\f0\\fs24 "); 
    
    size_t len = strlen(text);
    for (size_t i = 0; i < len; i++) {
        TextStyle style;
        bool has_style = formatter_get_style_at(ctx->formatter, i, &style);
        
        if (has_style) {
            if (style.bold) fprintf(file, "\\b ");
            if (style.italic) fprintf(file, "\\i ");
            
            if (style.fg_color == 0x0000FF) fprintf(file, "\\cf1 ");
            else if (style.fg_color == 0x008000) fprintf(file, "\\cf2 ");
            else if (style.fg_color == 0xA31515) fprintf(file, "\\cf3 ");
            else if (style.fg_color == 0x7F0055) fprintf(file, "\\cf4 ");
            else fprintf(file, "\\cf0 ");
        } else {
            fprintf(file, "\\cf0\\b0\\i0 ");
        }
        
        if (text[i] == '\n') fprintf(file, "\\par\n");
        else if (text[i] == '\\' || text[i] == '{' || text[i] == '}') fprintf(file, "\\%c", text[i]);
        else if (text[i] == '\t') fprintf(file, "\\tab ");
        else fputc(text[i], file);
        
        if (has_style) {
            if (style.bold) fprintf(file, "\\b0 ");
            if (style.italic) fprintf(file, "\\i0 ");
        }
    }
    
    fprintf(file, "}\n");
    
    free(text);
    fclose(file);
    return true;
}
