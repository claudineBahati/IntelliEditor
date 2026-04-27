#include "rules_panel.h"

HWND RulesPanel_Create(HWND hParent, int x, int y, int width, int height, int id) {
    HWND hList = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "LISTBOX",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
        x, y, width, height,
        hParent,
        (HMENU)(UINT_PTR)id,
        GetModuleHandle(NULL),
        NULL
    );

    if (hList) {
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"[OK] R001 - Introduction");
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"[WARN] R002 - 1ere personne");
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)"[ERR] R003 - Titre H1");
    }

    return hList;
}
