#include <windows.h>
#include <commctrl.h>
#include "scintilla_wrapper.h"
#include "toolbar.h"
#include "statusbar.h"
#include "rules_panel.h"

// Déclaration de l'éditeur Scintilla
static HWND hEditor = NULL;
static HWND hToolbar = NULL;
static HWND hStatusbar = NULL;
static HWND hRulesPanel = NULL;

// Fonction qui gère les messages de la fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Créer la barre d'outils
            hToolbar = Toolbar_Create(hwnd);

            // Créer la barre de statut
            hStatusbar = Statusbar_Create(hwnd);

            // Créer le panneau des règles (à droite)
            hRulesPanel = RulesPanel_Create(hwnd, 0, 0, 0, 0, 4);

            // Créer la zone Scintilla via le wrapper
            hEditor = Scintilla_Create(hwnd, 0, 0, 0, 0, 1);
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
AppendMenu(hFileMenu, MF_STRING, 1, "Nouveau");
AppendMenu(hFileMenu, MF_STRING, 2, "Ouvrir");
AppendMenu(hFileMenu, MF_STRING, 3, "Sauvegarder");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "Fichier");

// Menu Édition
HMENU hEditMenu = CreatePopupMenu();
AppendMenu(hEditMenu, MF_STRING, 4, "Copier");
AppendMenu(hEditMenu, MF_STRING, 5, "Coller");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "Édition");

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
