#include "dialogs.h"
#include "scintilla_wrapper.h"
#include <stdio.h>
#include <string.h>

#define IDC_SETTINGS_FONT   3010
#define IDC_SETTINGS_SIZE   3011
#define IDC_SETTINGS_MARGIN 3012
#define IDC_SETTINGS_DARK   3013
#define IDC_SETTINGS_LINENO 3014

bool Dialog_OpenFile(HWND hParent, char* outFilePath, int maxLen) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hParent;
    ofn.lpstrFile = outFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = maxLen;
    ofn.lpstrFilter = "Fichiers texte (*.txt)\0*.txt\0Règles JSON (*.json)\0*.json\0Tous les fichiers (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        return true;
    }
    return false;
}

bool Dialog_SaveFile(HWND hParent, char* outFilePath, int maxLen) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hParent;
    ofn.lpstrFile = outFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = maxLen;
    ofn.lpstrFilter = "Fichiers texte (*.txt)\0*.txt\0Tous les fichiers (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        return true;
    }
    return false;
}

static BOOL CALLBACK SetChildFont(HWND hwndChild, LPARAM lParam) {
    SendMessage(hwndChild, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HFONT hDlgFont = NULL;
    switch (msg) {
        case WM_CREATE: {
            // Créer les étiquettes et contrôles
            CreateWindow("STATIC", "Police :", WS_CHILD | WS_VISIBLE, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
            HWND hComboFont = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 130, 18, 150, 100, hwnd, (HMENU)IDC_SETTINGS_FONT, NULL, NULL);
            SendMessage(hComboFont, CB_ADDSTRING, 0, (LPARAM)"Calibri");
            SendMessage(hComboFont, CB_ADDSTRING, 0, (LPARAM)"Arial");
            SendMessage(hComboFont, CB_ADDSTRING, 0, (LPARAM)"Consolas");
            SendMessage(hComboFont, CB_ADDSTRING, 0, (LPARAM)"Segoe UI");
            
            // Sélectionner la police actuelle
            int idxFont = 0;
            if (strcmp(g_fontName, "Arial") == 0) idxFont = 1;
            else if (strcmp(g_fontName, "Consolas") == 0) idxFont = 2;
            else if (strcmp(g_fontName, "Segoe UI") == 0) idxFont = 3;
            SendMessage(hComboFont, CB_SETCURSEL, idxFont, 0);

            CreateWindow("STATIC", "Taille :", WS_CHILD | WS_VISIBLE, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
            HWND hComboSize = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 130, 58, 150, 100, hwnd, (HMENU)IDC_SETTINGS_SIZE, NULL, NULL);
            SendMessage(hComboSize, CB_ADDSTRING, 0, (LPARAM)"10");
            SendMessage(hComboSize, CB_ADDSTRING, 0, (LPARAM)"12");
            SendMessage(hComboSize, CB_ADDSTRING, 0, (LPARAM)"14");
            SendMessage(hComboSize, CB_ADDSTRING, 0, (LPARAM)"16");
            
            int idxSize = 1; // par défaut 12
            if (g_fontSize == 10) idxSize = 0;
            else if (g_fontSize == 14) idxSize = 2;
            else if (g_fontSize == 16) idxSize = 3;
            SendMessage(hComboSize, CB_SETCURSEL, idxSize, 0);

            CreateWindow("STATIC", "Marges :", WS_CHILD | WS_VISIBLE, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
            HWND hComboMargin = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 130, 98, 150, 100, hwnd, (HMENU)IDC_SETTINGS_MARGIN, NULL, NULL);
            SendMessage(hComboMargin, CB_ADDSTRING, 0, (LPARAM)"Etroites (20px)");
            SendMessage(hComboMargin, CB_ADDSTRING, 0, (LPARAM)"Normales (60px)");
            SendMessage(hComboMargin, CB_ADDSTRING, 0, (LPARAM)"Larges (100px)");
            
            int idxMargin = 1; // par défaut Normales
            if (g_pageMargin == 20) idxMargin = 0;
            else if (g_pageMargin == 100) idxMargin = 2;
            SendMessage(hComboMargin, CB_SETCURSEL, idxMargin, 0);

            HWND hCheckDark = CreateWindow("BUTTON", "Mode Sombre", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, 140, 110, 20, hwnd, (HMENU)IDC_SETTINGS_DARK, NULL, NULL);
            SendMessage(hCheckDark, BM_SETCHECK, bDarkMode ? BST_CHECKED : BST_UNCHECKED, 0);

            HWND hCheckLineNo = CreateWindow("BUTTON", "Numeros de lignes", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 140, 140, 150, 20, hwnd, (HMENU)IDC_SETTINGS_LINENO, NULL, NULL);
            SendMessage(hCheckLineNo, BM_SETCHECK, bShowLineNumbers ? BST_CHECKED : BST_UNCHECKED, 0);

            // Boutons d'action
            CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 60, 185, 80, 30, hwnd, (HMENU)IDOK, NULL, NULL);
            CreateWindow("BUTTON", "Annuler", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, 185, 80, 30, hwnd, (HMENU)IDCANCEL, NULL, NULL);

            // Appliquer une police système propre (Segoe UI) aux étiquettes et contrôles
            hDlgFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
            EnumChildWindows(hwnd, SetChildFont, (LPARAM)hDlgFont);
            break;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDOK) {
                HWND hComboFont = GetDlgItem(hwnd, IDC_SETTINGS_FONT);
                HWND hComboSize = GetDlgItem(hwnd, IDC_SETTINGS_SIZE);
                HWND hComboMargin = GetDlgItem(hwnd, IDC_SETTINGS_MARGIN);
                HWND hCheckDark = GetDlgItem(hwnd, IDC_SETTINGS_DARK);
                HWND hCheckLineNo = GetDlgItem(hwnd, IDC_SETTINGS_LINENO);

                char fontName[32];
                int selFont = (int)SendMessage(hComboFont, CB_GETCURSEL, 0, 0);
                SendMessage(hComboFont, CB_GETLBTEXT, selFont, (LPARAM)fontName);
                strcpy(g_fontName, fontName);

                int selSize = (int)SendMessage(hComboSize, CB_GETCURSEL, 0, 0);
                if (selSize == 0) g_fontSize = 10;
                else if (selSize == 1) g_fontSize = 12;
                else if (selSize == 2) g_fontSize = 14;
                else if (selSize == 3) g_fontSize = 16;

                int selMargin = (int)SendMessage(hComboMargin, CB_GETCURSEL, 0, 0);
                if (selMargin == 0) g_pageMargin = 20;
                else if (selMargin == 1) g_pageMargin = 60;
                else if (selMargin == 2) g_pageMargin = 100;

                bDarkMode = (SendMessage(hCheckDark, BM_GETCHECK, 0, 0) == BST_CHECKED);
                bShowLineNumbers = (SendMessage(hCheckLineNo, BM_GETCHECK, 0, 0) == BST_CHECKED);

                DestroyWindow(hwnd);
            } else if (id == IDCANCEL) {
                DestroyWindow(hwnd);
            }
            break;
        }
        case WM_DESTROY: {
            if (hDlgFont) {
                DeleteObject(hDlgFont);
                hDlgFont = NULL;
            }
            break;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void Dialogs_ShowSettings(HWND hParent) {
    const char CLASS_NAME[] = "IntelliEditorSettingsWnd";
    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASS wc = {0};
    if (!GetClassInfo(hInst, CLASS_NAME, &wc)) {
        wc.lpfnWndProc   = SettingsWndProc;
        wc.hInstance     = hInst;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        RegisterClass(&wc);
    }

    // Centrer le dialogue par rapport au parent
    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int pWidth = rcParent.right - rcParent.left;
    int pHeight = rcParent.bottom - rcParent.top;
    int dWidth = 320;
    int dHeight = 270;
    int x = rcParent.left + (pWidth - dWidth) / 2;
    int y = rcParent.top + (pHeight - dHeight) / 2;

    HWND hwndDlg = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        "Parametres",
        WS_POPUPWINDOW | WS_CAPTION,
        x, y, dWidth, dHeight,
        hParent,
        NULL,
        hInst,
        NULL
    );

    if (hwndDlg) {
        // Rendre le dialogue modal
        EnableWindow(hParent, FALSE);
        ShowWindow(hwndDlg, SW_SHOW);
        UpdateWindow(hwndDlg);

        // Boucle de messages locale
        MSG msg;
        while (IsWindow(hwndDlg) && GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Réactiver le parent
        EnableWindow(hParent, TRUE);
        SetFocus(hParent);
        
        // Envoyer une commande personnalisée au parent pour appliquer les nouveaux réglages
        SendMessage(hParent, WM_COMMAND, 3002, 0);
    }
}
