#ifndef SCINTILLA_WRAPPER_H
#define SCINTILLA_WRAPPER_H

#include <windows.h>

// Initialise la bibliothèque Scintilla (charge la DLL)
BOOL Scintilla_Init(void);

// Crée le composant Scintilla dans la fenêtre parente
HWND Scintilla_Create(HWND hParent, int x, int y, int width, int height, int id);

// Configure les propriétés par défaut (police, numéros de ligne)
void Scintilla_Configure(HWND hScintilla);

// Manipulation du texte
void Scintilla_SetText(HWND hScintilla, const char* text);
void Scintilla_GetText(HWND hScintilla, char* buffer, int len);
int Scintilla_GetTextLength(HWND hScintilla);
void Scintilla_Clear(HWND hScintilla);
void Scintilla_Copy(HWND hScintilla);
void Scintilla_Paste(HWND hScintilla);

// Thèmes et Affichage
void Scintilla_SetTheme(HWND hScintilla, BOOL darkMode);
void Scintilla_AddError(HWND hScintilla, int start, int length);
void Scintilla_ClearErrors(HWND hScintilla);

#endif // SCINTILLA_WRAPPER_H
