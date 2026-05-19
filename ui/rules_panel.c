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
        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        SendMessage(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    return hList;
}

void RulesPanel_AddRule(HWND hRulesPanel, const char* ruleText) {
    if (hRulesPanel && ruleText) {
        SendMessage(hRulesPanel, LB_ADDSTRING, 0, (LPARAM)ruleText);
    }
}

void RulesPanel_Clear(HWND hRulesPanel) {
    if (hRulesPanel) {
        SendMessage(hRulesPanel, LB_RESETCONTENT, 0, 0);
    }
}
