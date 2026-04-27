#ifndef SCINTILLA_WRAPPER_H
#define SCINTILLA_WRAPPER_H

#include <windows.h>

// Initialise la bibliothèque Scintilla (charge la DLL)
BOOL Scintilla_Init(void);

// Crée le composant Scintilla dans la fenêtre parente
HWND Scintilla_Create(HWND hParent, int x, int y, int width, int height, int id);

// Configure les propriétés par défaut (police, numéros de ligne)
void Scintilla_Configure(HWND hScintilla);

#endif // SCINTILLA_WRAPPER_H
