#include <windows.h>
#include <commctrl.h>
#include "scintilla_wrapper.h"
#include "toolbar.h"
#include "statusbar.h"
#include "rules_panel.h"
#include "dialogs.h"
#include "resources.h"
#include <stdio.h>
#include <Scintilla.h>

// Déclaration de l'éditeur Scintilla
static HWND hEditor = NULL;
static HWND hToolbar = NULL;
static HWND hStatusbar = NULL;
static HWND hRulesPanel = NULL;
static BOOL bDarkMode = FALSE;

// Fonction utilitaire pour charger un fichier dans Scintilla
void LoadFileToEditor(HWND hwnd, const char* szFileName) {
    FILE* file = fopen(szFileName, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* buffer = malloc(length + 1);
        if (buffer) {
            fread(buffer, 1, length, file);
            buffer[length] = '\0';
            Scintilla_SetText(hEditor, buffer);
            free(buffer);
        }
        fclose(file);
    }
}

// Fonction qui gère les messages de la fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case IDM_FILE_NEW:
                    Scintilla_Clear(hEditor);
                    break;
                case IDM_FILE_OPEN: {
                    char szFileName[MAX_PATH] = {0};
                    if (Dialog_OpenFile(hwnd, szFileName, MAX_PATH)) {
                        LoadFileToEditor(hwnd, szFileName);
                    }
                    break;
                }
                case IDM_FILE_SAVE: {
                    char szFileName[MAX_PATH] = {0};
                    if (Dialog_SaveFile(hwnd, szFileName, MAX_PATH)) {
                        FILE* file = fopen(szFileName, "wb");
                        if (file) {
                            int length = Scintilla_GetTextLength(hEditor);
                            char* buffer = malloc(length + 1);
                            if (buffer) {
                                Scintilla_GetText(hEditor, buffer, length + 1);
                                fwrite(buffer, 1, length, file);
                                free(buffer);
                            }
                            fclose(file);
                        }
                    }
                    break;
                }
                case IDM_EDIT_COPY:
                    Scintilla_Copy(hEditor);
                    break;
                case IDM_EDIT_PASTE:
                    Scintilla_Paste(hEditor);
                    break;
                case IDM_VIEW_DARKMODE:
                    bDarkMode = !bDarkMode;
                    Scintilla_SetTheme(hEditor, bDarkMode);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_DARKMODE, bDarkMode ? MF_CHECKED : MF_UNCHECKED);
                    break;
            }
            break;
        }
        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            char szFileName[MAX_PATH];
            if (DragQueryFile(hDrop, 0, szFileName, MAX_PATH)) {
                LoadFileToEditor(hwnd, szFileName);
            }
            DragFinish(hDrop);
            break;
        }
        case WM_NOTIFY: {
            NMHDR* phdr = (NMHDR*)lParam;
            if (phdr->hwndFrom == hEditor && phdr->code == SCN_UPDATEUI) {
                int pos = (int)SendMessage(hEditor, SCI_GETCURRENTPOS, 0, 0);
                int line = (int)SendMessage(hEditor, SCI_LINEFROMPOSITION, pos, 0);
                int col = (int)SendMessage(hEditor, SCI_GETCOLUMN, pos, 0);
                // On peut aussi calculer le nombre de mots ici si on veut
                Statusbar_Update(hStatusbar, 0, line + 1, col + 1);
            }
            break;
        }
        case WM_CREATE: {
            // Créer la barre d'outils
            hToolbar = Toolbar_Create(hwnd);

            // Créer la barre de statut
            hStatusbar = Statusbar_Create(hwnd);

            // Créer le panneau des règles (à droite)
            hRulesPanel = RulesPanel_Create(hwnd, 0, 0, 0, 0, ID_RULESPANEL);

            // Créer la zone Scintilla via le wrapper
            hEditor = Scintilla_Create(hwnd, 0, 0, 0, 0, ID_EDITOR);

            // Activer le Drag & Drop
            DragAcceptFiles(hwnd, TRUE);
            break;
        }

        case WM_SIZE: {
            // Redimensionner les composants
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            // 1. La barre d'outils s'adapte automatiquement
            SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
            
            RECT rcToolbar;
            GetWindowRect(hToolbar, &rcToolbar);
            int tbHeight = rcToolbar.bottom - rcToolbar.top;

            // 2. La barre de statut s'adapte automatiquement
            SendMessage(hStatusbar, WM_SIZE, 0, 0);
            
            RECT rcStatus;
            GetWindowRect(hStatusbar, &rcStatus);
            int sbHeight = rcStatus.bottom - rcStatus.top;

            // L'espace restant pour l'éditeur et le panneau
            int yEditor = tbHeight;
            int hEditorHeight = height - tbHeight - sbHeight;
            
            // Largeur du panneau des règles (ex: 250px)
            int rulesWidth = 250;
            int editorWidth = width - rulesWidth;

            // Positionner Scintilla et le panneau des règles
            if (hEditor) {
                SetWindowPos(hEditor, NULL, 0, yEditor, editorWidth, hEditorHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            if (hRulesPanel) {
                SetWindowPos(hRulesPanel, NULL, editorWidth, yEditor, rulesWidth, hEditorHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Point d’entrée du programme
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "IntelliEditorMainWindow";

    // Initialiser Scintilla avant de créer la fenêtre
    if (!Scintilla_Init()) {
        MessageBox(NULL, "Erreur : Impossible de charger SciLexer.dll / Scintilla.dll", "Erreur", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Définir la classe de fenêtre
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Créer la fenêtre principale
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "IntelliEditor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );
// Créer la barre de menus
HMENU hMenu = CreateMenu();

// Menu Fichier
HMENU hFileMenu = CreatePopupMenu();
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "Nouveau");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "Ouvrir");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "Sauvegarder");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "Fichier");

// Menu Édition
HMENU hEditMenu = CreatePopupMenu();
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_COPY, "Copier");
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_PASTE, "Coller");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "Édition");

// Menu Affichage
HMENU hViewMenu = CreatePopupMenu();
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_DARKMODE, "Mode Sombre");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, "Affichage");

// Attacher la barre de menus à la fenêtre
SetMenu(hwnd, hMenu);


    ShowWindow(hwnd, nCmdShow);

    // Boucle de messages
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
