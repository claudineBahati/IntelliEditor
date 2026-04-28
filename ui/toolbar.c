#include "toolbar.h"
#include <commctrl.h>

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

    TBBUTTON tbButtons[3] = {
        { STD_FILENEW, IDM_FILE_NEW, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)"Nouveau" },
        { STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)"Ouvrir" },
        { STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)"Sauvegarder" }
    };

    TBADDBITMAP tbab;
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(hToolBar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

    SendMessage(hToolBar, TB_ADDBUTTONS, (WPARAM)3, (LPARAM)&tbButtons);
    SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);

    return hToolBar;
}
