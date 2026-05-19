#define _WIN32_IE 0x0500
#include "toolbar.h"
#include <commctrl.h>

#ifndef TB_SETBKCOLOR
#define TB_SETBKCOLOR (WM_USER + 60)
#endif

HWND Toolbar_Create(HWND hParent) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hToolBar = CreateWindowEx(
        0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hParent, (HMENU)ID_TOOLBAR, GetModuleHandle(NULL), NULL
    );

    SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    TBBUTTON tbButtons[7] = {
        { STD_FILENEW, IDM_FILE_NEW, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0 },
        { STD_FIND, IDM_EDIT_FIND, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 },
        { 0, 0, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0 },
        { STD_PROPERTIES, IDM_VIEW_DARKMODE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, 0 }
    };

    TBADDBITMAP tbab;
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(hToolBar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

    SendMessage(hToolBar, TB_ADDBUTTONS, (WPARAM)7, (LPARAM)&tbButtons);
    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);

    return hToolBar;
}

void Toolbar_SetDarkMode(HWND hToolbar, BOOL bDark) {
    if (bDark) {
        SendMessage(hToolbar, TB_SETBKCOLOR, 0, (LPARAM)RGB(45, 45, 45));
    } else {
        SendMessage(hToolbar, TB_SETBKCOLOR, 0, (LPARAM)CLR_DEFAULT);
    }
    
    // Forcer le redessin
    InvalidateRect(hToolbar, NULL, TRUE);
}
