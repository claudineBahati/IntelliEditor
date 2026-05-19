#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <windows.h>
#include "resources.h"

HWND Toolbar_Create(HWND hParent);
void Toolbar_SetDarkMode(HWND hToolbar, BOOL bDark);

#endif // TOOLBAR_H
