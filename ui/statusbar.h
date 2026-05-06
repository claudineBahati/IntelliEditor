#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <windows.h>

HWND Statusbar_Create(HWND hParent);
void Statusbar_Update(HWND hStatusbar, int words, int line, int col);
void Statusbar_SetDarkMode(HWND hStatusbar, BOOL bDark);

#endif // STATUSBAR_H
