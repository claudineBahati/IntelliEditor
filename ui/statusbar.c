#include "statusbar.h"
#include "resources.h"
#include <commctrl.h>
#include <stdio.h>

HWND Statusbar_Create(HWND hParent) {
    HWND hStatus = CreateWindowEx(
        0, STATUSCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        hParent, (HMENU)ID_STATUSBAR, GetModuleHandle(NULL), NULL
    );

    int parts[4] = { 150, 350, 450, -1 };
    SendMessage(hStatus, SB_SETPARTS, 4, (LPARAM)parts);

    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Mots : 0");
    SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)"Ligne 1, Col 1");
    SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)"UTF-8 | FR");
    SendMessage(hStatus, SB_SETTEXT, 3, (LPARAM)"Pret");

    return hStatus;
}

void Statusbar_Update(HWND hStatusbar, int words, int line, int col, const char* encoding) {
    if (!hStatusbar) return;
    
    char bufWords[64];
    char bufPos[64];
    
    snprintf(bufWords, sizeof(bufWords), "Mots : %d", words);
    snprintf(bufPos, sizeof(bufPos), "Ligne %d, Col %d", line, col);
    
    SendMessage(hStatusbar, SB_SETTEXT, 0, (LPARAM)bufWords);
    SendMessage(hStatusbar, SB_SETTEXT, 1, (LPARAM)bufPos);
    if (encoding) {
        SendMessage(hStatusbar, SB_SETTEXT, 2, (LPARAM)encoding);
    }
}

void Statusbar_SetDarkMode(HWND hStatusbar, BOOL bDark) {
    if (!hStatusbar) return;
    COLORREF bg = bDark ? RGB(45, 45, 45) : CLR_DEFAULT;
    SendMessage(hStatusbar, SB_SETBKCOLOR, 0, (LPARAM)bg);
}
