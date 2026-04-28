#include "scintilla_wrapper.h"
#include <Scintilla.h>

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
    
    // Style par défaut
    SendMessage(hScintilla, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Consolas");
    SendMessage(hScintilla, SCI_STYLESETSIZE, STYLE_DEFAULT, 12);
    
    // Configurer la marge 0 pour afficher les numéros de ligne
    SendMessage(hScintilla, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    SendMessage(hScintilla, SCI_SETMARGINWIDTHN, 0, 40);
    
    // Définir l'encodage par défaut à UTF-8
    SendMessage(hScintilla, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
    
    // Activer le retour à la ligne automatique (word wrap)
    SendMessage(hScintilla, SCI_SETWRAPMODE, SC_WRAP_WORD, 0);

    // Configuration de l'indicateur d'erreur (Squiggle rouge)
    SendMessage(hScintilla, SCI_INDICSETSTYLE, 0, INDIC_SQUIGGLE);
    SendMessage(hScintilla, SCI_INDICSETFORE, 0, RGB(255, 0, 0)); // Rouge
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

void Scintilla_SetTheme(HWND hScintilla, BOOL darkMode) {
    if (!hScintilla) return;

    COLORREF bgColor = darkMode ? RGB(30, 30, 30) : RGB(255, 255, 255);
    COLORREF fgColor = darkMode ? RGB(220, 220, 220) : RGB(0, 0, 0);
    COLORREF caretColor = darkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);

    // Appliquer au style par défaut
    SendMessage(hScintilla, SCI_STYLESETBACK, STYLE_DEFAULT, bgColor);
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_DEFAULT, fgColor);
    
    // Forcer le rafraîchissement de tous les styles basés sur STYLE_DEFAULT
    SendMessage(hScintilla, SCI_STYLECLEARALL, 0, 0);

    // Couleurs spécifiques
    SendMessage(hScintilla, SCI_SETCARETFORE, caretColor, 0);
    
    // Marge des numéros de ligne
    SendMessage(hScintilla, SCI_STYLESETBACK, STYLE_LINENUMBER, darkMode ? RGB(45, 45, 45) : RGB(240, 240, 240));
    SendMessage(hScintilla, SCI_STYLESETFORE, STYLE_LINENUMBER, darkMode ? RGB(150, 150, 150) : RGB(100, 100, 100));
}

void Scintilla_AddError(HWND hScintilla, int start, int length) {
    if (!hScintilla) return;
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hScintilla, SCI_INDICATORFILLRANGE, start, length);
}

void Scintilla_ClearErrors(HWND hScintilla) {
    if (!hScintilla) return;
    int length = Scintilla_GetTextLength(hScintilla);
    SendMessage(hScintilla, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hScintilla, SCI_INDICATORCLEARRANGE, 0, length);
}
