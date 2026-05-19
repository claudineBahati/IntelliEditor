#include "tokenizer.h"
#include "debug_memory.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

static bool is_keyword(const char* word) {
    const char* keywords[] = {
        "int", "char", "float", "double", "void", "if", "else", "while", "for",
        "return", "break", "continue", "struct", "typedef", "static", "const",
        "unsigned", "sizeof", "include", "define", NULL
    };
    
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(word, keywords[i]) == 0) return true;
    }
    return false;
}

void tokenizer_run_c(EditorContext* ctx) {
    if (!ctx || !ctx->buffer || !ctx->formatter) return;
    
    formatter_clear_ranges(ctx->formatter);
    
    char* text = editor_get_text(ctx);
    if (!text) return;
    
    size_t len = strlen(text);
    size_t i = 0;
    
    TextStyle style_keyword = {0x0000FF, 0, true, false, false};  // Bleu, Gras
    TextStyle style_comment = {0x008000, 0, false, true, false}; // Vert, Italique
    TextStyle style_string = {0xA31515, 0, false, false, false};  // Rouge sombre
    TextStyle style_preproc = {0x7F0055, 0, true, false, false};  // Violet
    
    while (i < len) {
        // Ignorer les espaces
        if (isspace((unsigned char)text[i])) {
            i++;
            continue;
        }
        
        // Commentaires
        if (text[i] == '/' && i + 1 < len) {
            if (text[i+1] == '/') { // Commentaire ligne
                size_t start = i;
                while (i < len && text[i] != '\n') i++;
                formatter_add_range(ctx->formatter, start, i - start, style_comment);
                continue;
            } else if (text[i+1] == '*') { // Commentaire bloc
                size_t start = i;
                i += 2;
                while (i + 1 < len && !(text[i] == '*' && text[i+1] == '/')) i++;
                i += 2;
                formatter_add_range(ctx->formatter, start, i - start, style_comment);
                continue;
            }
        }
        
        // Chaînes de caractères
        if (text[i] == '\"') {
            size_t start = i;
            i++;
            while (i < len && text[i] != '\"') {
                if (text[i] == '\\' && i + 1 < len) i++; // Escape
                i++;
            }
            i++;
            formatter_add_range(ctx->formatter, start, i - start, style_string);
            continue;
        }
        
        // Préprocesseur
        if (text[i] == '#') {
            size_t start = i;
            while (i < len && !isspace((unsigned char)text[i])) i++;
            formatter_add_range(ctx->formatter, start, i - start, style_preproc);
            continue;
        }
        
        // Mots-clés et identifiants
        if (isalpha((unsigned char)text[i]) || text[i] == '_') {
            size_t start = i;
            while (i < len && (isalnum((unsigned char)text[i]) || text[i] == '_')) i++;
            
            size_t word_len = i - start;
            char word[64];
            if (word_len < 64) {
                strncpy(word, text + start, word_len);
                word[word_len] = '\0';
                if (is_keyword(word)) {
                    formatter_add_range(ctx->formatter, start, word_len, style_keyword);
                }
            }
            continue;
        }
        
        // Nombres
        if (isdigit((unsigned char)text[i])) {
            size_t start = i;
            while (i < len && isdigit((unsigned char)text[i])) i++;
            TextStyle style_number = {0x000000, 0xFFFF00, false, false, true}; // Noir sur jaune, Souligné
            formatter_add_range(ctx->formatter, start, i - start, style_number);
            continue;
        }
        
        // Opérateurs et ponctuation
        if (strchr("+-*/%=&|<>!;,.", text[i])) {
            TextStyle style_op = {0x7F0055, 0, false, false, false}; // Violet
            formatter_add_range(ctx->formatter, i, 1, style_op);
            i++;
            continue;
        }
        
        i++;
    }
    
    free(text);
}
