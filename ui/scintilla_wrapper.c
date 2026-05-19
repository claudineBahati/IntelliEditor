#include "scintilla_wrapper.h"
#include "spellcheck.h"
#include <Scintilla.h>
#include <ctype.h>
#include <stdlib.h>

static HMODULE hSciLexer = NULL;

BOOL Scintilla_Init(void) {
    if (hSciLexer == NULL) {
        hSciLexer = LoadLibrary("SciLexer.dll");
        if (hSciLexer == NULL) {
            hSciLexer = LoadLibrary("Scintilla.dll");
        }
    }
    return (hSciLexer != NULL);
}

HWND Scintilla_Create(HWND hParent, int x, int y, int width, int height, int id) {
    HWND hEditor = CreateWindowEx(
        0,
        "Scintilla",
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
        x, y, width, height,
        hParent,
        (HMENU)(UINT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );
    
    if (hEditor) {
        Scintilla_Configure(hEditor);
    }
    
    return hEditor;
}

void Scintilla_Configure(HWND hScintilla) {
    if (!hScintilla) return;
    
    // Style par défaut (MS Word-like dynamique)
    SendMessage(hScintilla, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)g_fontName);
    SendMessage(hScintilla, SCI_STYLESETSIZE, STYLE_DEFAULT, g_fontSize);
    
    // Configurer la marge de numéros de ligne (affichée ou cachée)
    SendMessage(hScintilla, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    SendMessage(hScintilla, SCI_SETMARGINWIDTHN, 0, bShowLineNumbers ? 40 : 0);
    
    // Marges intérieures gauche/droite pour simuler une page
    SendMessage(hScintilla, SCI_SETMARGINLEFT, 0, g_pageMargin);
    SendMessage(hScintilla, SCI_SETMARGINRIGHT, 0, g_pageMargin);
    
    // Interligne aéré (comme Word 1.15)
    SendMessage(hScintilla, SCI_SETEXTRAASCENT, 3, 0);
    SendMessage(hScintilla, SCI_SETEXTRADESCENT, 3, 0);
    
    // Définir l'encodage par défaut à UTF-8
    SendMessage(hScintilla, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
    
    // Activer le retour à la ligne automatique (word wrap)
    SendMessage(hScintilla, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);

    // Configurer la représentation visuelle du saut de page (\f)
    SendMessage(hScintilla, SCI_SETREPRESENTATION, (WPARAM)"\x0c", (LPARAM)" [--------------------------------------- Saut de page ---------------------------------------] ");

    // Mettre en évidence la ligne courante
    SendMessage(hScintilla, SCI_SETCARETLINEVISIBLE, TRUE, 0);
    SendMessage(hScintilla, SCI_SETCARETLINEBACK, RGB(240, 240, 240), 0);
    SendMessage(hScintilla, SCI_SETCARETLINEBACKALPHA, 50, 0);

    // Activer la correspondance des parenthèses/accolades
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_BRACELIGHT, RGB(255, 0, 0));
    SendMessage(hScintilla, SCI_STYLESETBOLD, STYLE_BRACELIGHT, TRUE);

    // Configuration de l'indicateur d'erreur d'orthographe (0 - Rouge)
    SendMessage(hScintilla, SCI_INDICSETSTYLE, 0, INDIC_SQUIGGLE);
    SendMessage(hScintilla, SCI_INDICSETFORE, 0, RGB(255, 0, 0));

    // Configuration de l'indicateur de grammaire/sémantique (1 - Vert)
    SendMessage(hScintilla, SCI_INDICSETSTYLE, 1, INDIC_SQUIGGLE);
    SendMessage(hScintilla, SCI_INDICSETFORE, 1, RGB(0, 150, 0));
}

void Scintilla_SetText(HWND hScintilla, const char* text) {
    SendMessage(hScintilla, SCI_SETTEXT, 0, (LPARAM)text);
}

void Scintilla_GetText(HWND hScintilla, char* buffer, int len) {
    SendMessage(hScintilla, SCI_GETTEXT, (WPARAM)len, (LPARAM)buffer);
}

int Scintilla_GetTextLength(HWND hScintilla) {
    return (int)SendMessage(hScintilla, SCI_GETTEXTLENGTH, 0, 0);
}

void Scintilla_Clear(HWND hScintilla) {
    SendMessage(hScintilla, SCI_CLEARALL, 0, 0);
}

void Scintilla_Copy(HWND hScintilla) {
    SendMessage(hScintilla, SCI_COPY, 0, 0);
}

void Scintilla_Paste(HWND hScintilla) {
    SendMessage(hScintilla, SCI_PASTE, 0, 0);
}

int Scintilla_GetWordCount(HWND hScintilla) {
    if (!hScintilla) return 0;
    int length = Scintilla_GetTextLength(hScintilla);
    if (length == 0) return 0;

    char* buffer = malloc(length + 1);
    if (!buffer) return 0;

    Scintilla_GetText(hScintilla, buffer, length + 1);

    int count = 0;
    BOOL inWord = FALSE;
    for (int i = 0; i < length; i++) {
        if (isspace((unsigned char)buffer[i])) {
            inWord = FALSE;
        } else {
            if (!inWord) {
                count++;
                inWord = TRUE;
            }
        }
    }

    free(buffer);
    return count;
}

void Scintilla_SetTheme(HWND hScintilla, BOOL darkMode) {
    if (!hScintilla) return;

    COLORREF bgColor = darkMode ? RGB(28, 30, 38) : RGB(255, 255, 255);
    COLORREF fgColor = darkMode ? RGB(220, 220, 225) : RGB(0, 0, 0);
    COLORREF caretColor = darkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
    COLORREF selColor = darkMode ? RGB(60, 70, 90) : RGB(200, 220, 255);
    COLORREF lineNumBg = darkMode ? RGB(35, 38, 48) : RGB(240, 240, 240);
    COLORREF lineNumFg = darkMode ? RGB(100, 110, 130) : RGB(128, 128, 128);
    COLORREF caretLineBg = darkMode ? RGB(40, 44, 55) : RGB(240, 242, 250);
    
    // Appliquer les couleurs de base
    SendMessage(hScintilla, SCI_STYLESETBACK, STYLE_DEFAULT, bgColor);
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_DEFAULT, fgColor);
    SendMessage(hScintilla, SCI_STYLECLEARALL, 0, 0);
 
    // Couleurs spécifiques
    SendMessage(hScintilla, SCI_SETCARETFORE, caretColor, 0);
    SendMessage(hScintilla, SCI_SETCARETLINEBACK, caretLineBg, 0);
    SendMessage(hScintilla, SCI_SETCARETLINEBACKALPHA, 100, 0);
    SendMessage(hScintilla, SCI_SETSELBACK, TRUE, selColor);
    
    // Marge des numéros de ligne
    SendMessage(hScintilla, SCI_STYLESETBACK, STYLE_LINENUMBER, lineNumBg);
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_LINENUMBER, lineNumFg);
    
    // Correspondance des parenthèses
    COLORREF braceColor = darkMode ? RGB(0, 255, 150) : RGB(255, 0, 0);
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_BRACELIGHT, braceColor);
    SendMessage(hScintilla, SCI_STYLESETBOLD, STYLE_BRACELIGHT, TRUE);
}

void Scintilla_AddError(HWND hScintilla, int start, int length) {
    if (!hScintilla) return;
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hScintilla, SCI_INDICATORFILLRANGE, start, length);
}

void Scintilla_ClearErrors(HWND hScintilla) {
    int length = (int)SendMessage(hScintilla, SCI_GETTEXTLENGTH, 0, 0);
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hScintilla, SCI_INDICATORCLEARRANGE, 0, length);
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 1, 0); // Pour la grammaire
    SendMessage(hScintilla, SCI_INDICATORCLEARRANGE, 0, length);
}

void Scintilla_ApplyNLPReport(HWND hScintilla, void* report_ptr) {
    if (!hScintilla) return;
    
    // Effacer les anciens indicateurs
    Scintilla_ClearErrors(hScintilla);

    int length = (int)SendMessage(hScintilla, SCI_GETTEXTLENGTH, 0, 0);
    if (length <= 0) return;

    char* text = malloc(length + 1);
    if (!text) return;
    SendMessage(hScintilla, SCI_GETTEXT, length + 1, (LPARAM)text);

    // Définir l'indicateur d'orthographe actif (0)
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 0, 0);

    // Parcourir le texte pour trouver les mots et leurs positions exactes
    int wordStart = -1;
    for (int i = 0; i <= length; i++) {
        char c = text[i];
        
        // Délimiteurs de mots incluant ponctuation, espaces et symboles
        BOOL isDelim = (c == '\0' || strchr(" ,.;:!?\n\r\t\"()[]{}<>=\\/*+-&|%^~`'\"", c) != NULL);

        if (!isDelim) {
            if (wordStart == -1) {
                wordStart = i; // Début du mot
            }
        } else {
            if (wordStart != -1) {
                int wordLen = i - wordStart;
                if (wordLen > 0 && wordLen < 64) {
                    char word[64];
                    memcpy(word, text + wordStart, wordLen);
                    word[wordLen] = '\0';

                    // S'assurer que le token contient au moins une lettre pour éviter de souligner la ponctuation/nombres
                    BOOL hasLetter = FALSE;
                    for (int k = 0; k < wordLen; k++) {
                        if (isalpha((unsigned char)word[k]) || (unsigned char)word[k] >= 128) {
                            hasLetter = TRUE;
                            break;
                        }
                    }

                    if (hasLetter) {
                        // Si le mot est incorrect dans notre correcteur d'orthographe
                        if (!spellcheck_is_correct(word)) {
                            SendMessage(hScintilla, SCI_INDICATORFILLRANGE, wordStart, wordLen);
                        }
                    }
                }
                wordStart = -1;
            }
        }
    }

    // Gérer les erreurs de grammaire/sémantique (Indicateur 1 - Vert)
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 1, 0);
    typedef struct {
        char* word;
        int word_index;
        char** suggestions;
        int suggest_count;
    } SpellingError;

    typedef struct {
        char original_phrase[256];
        char suggested_correction[256];
        char error_type[64];
    } GrammarError;

    typedef struct {
        SpellingError* sp_errors;
        int sp_error_count;
        GrammarError* gr_errors;
        int gr_error_count;
    } CorrectionReport;

    CorrectionReport* report = (CorrectionReport*)report_ptr;
    if (report && report->gr_errors) {
        for (int i = 0; i < report->gr_error_count; i++) {
            // Surligner la première phrase incorrecte si présente dans le texte
            const char* phrase = report->gr_errors[i].original_phrase;
            if (strlen(phrase) > 0) {
                char* p = strstr(text, phrase);
                if (p) {
                    int start = (int)(p - text);
                    SendMessage(hScintilla, SCI_INDICATORFILLRANGE, start, (int)strlen(phrase));
                }
            }
        }
    }

    free(text);
}

void Scintilla_UpdateBraceMatch(HWND hScintilla) {
    if (!hScintilla) return;
    int pos = (int)SendMessage(hScintilla, SCI_GETCURRENTPOS, 0, 0);
    int charAtPos = (int)SendMessage(hScintilla, SCI_GETCHARAT, pos, 0);
    
    // Vérifier si le caractère est une parenthèse/accolade
    if (strchr("()[]{}", charAtPos)) {
        int match = (int)SendMessage(hScintilla, SCI_BRACEMATCH, pos, 0);
        if (match != -1) {
            SendMessage(hScintilla, SCI_BRACEHIGHLIGHT, pos, match);
        } else {
            SendMessage(hScintilla, SCI_BRACEBADLIGHT, pos, 0);
        }
    } else {
        SendMessage(hScintilla, SCI_BRACEHIGHLIGHT, -1, -1);
    }
}
