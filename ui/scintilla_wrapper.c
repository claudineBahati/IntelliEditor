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
}
