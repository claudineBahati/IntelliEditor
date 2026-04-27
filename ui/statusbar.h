#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <windows.h>

HWND Statusbar_Create(HWND hParent);
void Statusbar_Update(HWND hStatusbar, int words, int line, int col);

#endif // STATUSBAR_H
