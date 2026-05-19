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
#include "rule_parser.h"
#include "nlp/nlp_engine.h"

RuleSet* load_rules(const char* file_path) {
    RuleSet* rs = malloc(sizeof(RuleSet));
    if (!rs) return NULL;
    if (load_rules_from_file(file_path, rs) == 1) {
        return rs;
    }
    free(rs);
    return NULL;
}

void free_ruleset(RuleSet* ruleset) {
    free(ruleset);
}

// Déclaration de l'éditeur Scintilla
static HWND hEditor = NULL;
static HWND hToolbar = NULL;
static HWND hStatusbar = NULL;
static HWND hRulesPanel = NULL;
static HWND hTabCtrl = NULL;
static HWND hSearchBar = NULL;
static HWND hSearchEdit = NULL;
static HWND hReplaceEdit = NULL;
static HWND hBtnNext = NULL;
static HWND hBtnReplace = NULL;
static HWND hBtnReplaceAll = NULL;
static RuleSet* g_ruleset = NULL;
static BOOL bDarkMode = FALSE;
static BOOL bShowSearchBar = FALSE;
static HBRUSH hbrDarkBackground = NULL;

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
            
            // Appliquer les règles après chargement
            RuleSet* ruleset = load_rules("data/rules.json");
            if (ruleset) {
                apply_rules(hEditor, ruleset);
                free_ruleset(ruleset);
            }
        }
        fclose(file);
    }
}

// Fonction qui gère les messages de la fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            if (bDarkMode) {
                HDC hdc = (HDC)wParam;
                SetTextColor(hdc, RGB(220, 220, 220));
                SetBkColor(hdc, RGB(45, 45, 45));
                return (LRESULT)hbrDarkBackground;
            }
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);

            // Gestion de la sélection dans le panneau des règles
            if (wmId == ID_RULES_PANEL && wmEvent == LBN_SELCHANGE) {
                int idx = (int)SendMessage(hRulesPanel, LB_GETCURSEL, 0, 0);
                if (idx != LB_ERR && g_ruleset && idx < g_ruleset->count) {
                    Rule r = g_ruleset->rules[idx];
                    if (strlen(r.parameter) > 0) {
                        const char* pattern = r.parameter;
                        int len = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
                        char* txt = malloc(len + 1);
                        SendMessage(hEditor, SCI_GETTEXT, len + 1, (LPARAM)txt);
                        char* p = strstr(txt, pattern);
                        if (p) {
                            int start = (int)(p - txt);
                            SendMessage(hEditor, SCI_SETSEL, start, start + (int)strlen(pattern));
                            SendMessage(hEditor, SCI_SCROLLCARET, 0, 0);
                        }
                        free(txt);
                    }
                }
            }

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
                case IDM_EDIT_FIND:
                    bShowSearchBar = !bShowSearchBar;
                    ShowWindow(hSearchBar, bShowSearchBar ? SW_SHOW : SW_HIDE);
                    // Déclencher un redimensionnement pour faire de la place
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
                    if (bShowSearchBar) SetFocus(hSearchEdit);
                    break;
                case 1009: { // Bouton 'Suivant'
                    char szSearch[256];
                    GetWindowText(hSearchEdit, szSearch, sizeof(szSearch));
                    if (strlen(szSearch) > 0) {
                        // Utiliser Scintilla pour chercher le texte
                        SendMessage(hEditor, SCI_SEARCHANCHOR, 0, 0);
                        int pos = (int)SendMessage(hEditor, SCI_SEARCHNEXT, SCFIND_NONE, (LPARAM)szSearch);
                        if (pos != -1) {
                            SendMessage(hEditor, SCI_SCROLLCARET, 0, 0);
                        } else {
                            // Recommencer du début
                            SendMessage(hEditor, SCI_SETSEL, 0, 0);
                            SendMessage(hEditor, SCI_SEARCHANCHOR, 0, 0);
                            SendMessage(hEditor, SCI_SEARCHNEXT, SCFIND_NONE, (LPARAM)szSearch);
                        }
                    }
                    break;
                }
                case 1010: { // Bouton 'Remplacer'
                    char szSearch[256], szReplace[256];
                    GetWindowText(hSearchEdit, szSearch, sizeof(szSearch));
                    GetWindowText(hReplaceEdit, szReplace, sizeof(szReplace));
                    if (strlen(szSearch) > 0) {
                        SendMessage(hEditor, SCI_TARGETFROMSELECTION, 0, 0);
                        SendMessage(hEditor, SCI_REPLACETARGET, strlen(szReplace), (LPARAM)szReplace);
                        // Chercher l'occurrence suivante
                        SendMessage(hwnd, WM_COMMAND, 1009, 0);
                    }
                    break;
                }
                case 1011: { // Bouton 'Tous' (Replace All)
                    char szSearch[256], szReplace[256];
                    GetWindowText(hSearchEdit, szSearch, sizeof(szSearch));
                    GetWindowText(hReplaceEdit, szReplace, sizeof(szReplace));
                    if (strlen(szSearch) > 0) {
                        SendMessage(hEditor, SCI_BEGINUNDOACTION, 0, 0);
                        int pos = 0;
                        int length = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
                        while (pos < length) {
                            SendMessage(hEditor, SCI_SETTARGETSTART, pos, 0);
                            SendMessage(hEditor, SCI_SETTARGETEND, length, 0);
                            int found = (int)SendMessage(hEditor, SCI_SEARCHINTARGET, strlen(szSearch), (LPARAM)szSearch);
                            if (found == -1) break;
                            SendMessage(hEditor, SCI_REPLACETARGET, strlen(szReplace), (LPARAM)szReplace);
                            pos = (int)SendMessage(hEditor, SCI_GETTARGETEND, 0, 0);
                            length = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
                        }
                        SendMessage(hEditor, SCI_ENDUNDOACTION, 0, 0);
                    }
                    break;
                }
                case 1020:
                case 1021:
                case 1022:
                    MessageBox(hwnd, "Module IA (Dev-C) en cours d'intégration...", "Intelligence Artificielle", MB_OK | MB_ICONINFORMATION);
                    break;
                case ID_RULES_PANEL: {
                    if (HIWORD(wParam) == LBN_SELCHANGE) {
                        int index = (int)SendMessage(hRulesPanel, LB_GETCURSEL, 0, 0);
                        if (index != LB_ERR && g_ruleset && index < g_ruleset->count) {
                            Rule* r = &g_ruleset->rules[index];
                            if (strlen(r->parameter) > 0) {
                                // Mettre le texte dans la barre de recherche et chercher
                                SetWindowText(hSearchEdit, r->parameter);
                                SendMessage(hwnd, WM_COMMAND, 1009, 0); // Simuler clic 'Suivant'
                            }
                        }
                    }
                    break;
                }
                case IDM_VIEW_DARKMODE:
                    bDarkMode = !bDarkMode;
                    Scintilla_SetTheme(hEditor, bDarkMode);
                    Toolbar_SetDarkMode(hToolbar, bDarkMode);
                    Statusbar_SetDarkMode(hStatusbar, bDarkMode);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_DARKMODE, bDarkMode ? MF_CHECKED : MF_UNCHECKED);
                    
                    InvalidateRect(hwnd, NULL, TRUE);
                    InvalidateRect(hToolbar, NULL, TRUE);
                    InvalidateRect(hStatusbar, NULL, TRUE);
                    InvalidateRect(hRulesPanel, NULL, TRUE);
                    break;

                case IDM_VIEW_SETTINGS:
                    Dialogs_ShowSettings(hwnd);
                    break;
            }
            break;
        }
        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            char szFileName[MAX_PATH];
            if (DragQueryFile(hDrop, 0, szFileName, MAX_PATH)) {
                if (strstr(szFileName, ".json")) {
                    // C'est probablement un fichier de règles
                    RuleSet* new_rules = load_rules(szFileName);
                    if (new_rules) {
                        if (g_ruleset) free_ruleset(g_ruleset);
                        g_ruleset = new_rules;
                        RulesPanel_Clear(hRulesPanel);
                        for (int i = 0; i < g_ruleset->count; i++) {
                            char buffer[256];
                            snprintf(buffer, sizeof(buffer), "[%s] %s", g_ruleset->rules[i].id, g_ruleset->rules[i].description);
                            RulesPanel_AddRule(hRulesPanel, buffer);
                        }
                        apply_rules(hEditor, g_ruleset);
                        MessageBox(hwnd, "Nouvelles règles chargées avec succès !", "Règles métier", MB_OK | MB_ICONINFORMATION);
                    }
                } else {
                    LoadFileToEditor(hwnd, szFileName);
                }
            }
            DragFinish(hDrop);
            break;
        }
        case WM_NOTIFY: {
            NMHDR* phdr = (NMHDR*)lParam;
            
            // Gestion Scintilla
            if (phdr->hwndFrom == hEditor && phdr->code == SCN_UPDATEUI) {
                int pos = (int)SendMessage(hEditor, SCI_GETCURRENTPOS, 0, 0);
                int line = (int)SendMessage(hEditor, SCI_LINEFROMPOSITION, pos, 0);
                int col = (int)SendMessage(hEditor, SCI_GETCOLUMN, pos, 0);
                int words = Scintilla_GetWordCount(hEditor);
                Statusbar_Update(hStatusbar, words, line + 1, col + 1, "UTF-8");
                Scintilla_UpdateBraceMatch(hEditor);

                // Analyse NLP (Phase 3)
                int len = Scintilla_GetTextLength(hEditor);
                if (len > 0) {
                    char* txt = malloc(len + 1);
                    Scintilla_GetText(hEditor, txt, len + 1);
                    CorrectionReport report = nlp_process_text(txt);
                    Scintilla_ApplyNLPReport(hEditor, &report);
                    nlp_free_report(&report);
                    free(txt);
                }
            }
            
            // Custom Draw pour le look Premium (Toolbar et Statusbar)
            if (bDarkMode && (phdr->code == NM_CUSTOMDRAW)) {
                LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
                if (phdr->hwndFrom == hToolbar || phdr->hwndFrom == hStatusbar) {
                    if (lpnmcd->dwDrawStage == CDDS_PREPAINT) {
                        // Fond sombre pour Toolbar/Statusbar
                        FillRect(lpnmcd->hdc, &lpnmcd->rc, hbrDarkBackground);
                        return CDRF_NOTIFYPOSTPAINT;
                    }
                    if (lpnmcd->dwDrawStage == CDDS_ITEMPREPAINT) {
                        // Couleur du texte en blanc pour les éléments
                        SetTextColor(lpnmcd->hdc, RGB(220, 220, 225));
                        SetBkMode(lpnmcd->hdc, TRANSPARENT);
                        return CDRF_DODEFAULT;
                    }
                }
            }
            break;
        }
        case WM_CREATE: {
            // Créer la barre d'outils
            hToolbar = Toolbar_Create(hwnd);

            // Créer la barre de statut
            hStatusbar = Statusbar_Create(hwnd);

            // Créer le pinceau pour le fond sombre
            hbrDarkBackground = CreateSolidBrush(RGB(45, 45, 45));

            // Créer le panneau des règles (à droite)
            hRulesPanel = RulesPanel_Create(hwnd, 0, 0, 0, 0, ID_RULES_PANEL);

            // Charger les règles métier
            g_ruleset = load_rules("data/rules.json");
            if (g_ruleset) {
                RulesPanel_Clear(hRulesPanel);
                for (int i = 0; i < g_ruleset->count; i++) {
                    char buffer[256];
                    snprintf(buffer, sizeof(buffer), "[%s] %s", g_ruleset->rules[i].id, g_ruleset->rules[i].description);
                    RulesPanel_AddRule(hRulesPanel, buffer);
                }
                // Appliquer les règles initiales (si texte présent)
                apply_rules(hEditor, g_ruleset);
            }

            // Créer la zone Scintilla via le wrapper
            hEditor = Scintilla_Create(hwnd, 0, 0, 0, 0, ID_EDITOR);

            // Créer les onglets (Phase 3)
            hTabCtrl = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)2006, GetModuleHandle(NULL), NULL);
            TCITEM tie;
            tie.mask = TCIF_TEXT;
            tie.pszText = "Document 1";
            SendMessage(hTabCtrl, TCM_INSERTITEM, 0, (LPARAM)&tie);
            tie.pszText = "+";
            SendMessage(hTabCtrl, TCM_INSERTITEM, 1, (LPARAM)&tie);

            // Créer la barre de recherche (cachée au début)
            hSearchBar = CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_BORDER, 0, 0, 0, 0, hwnd, NULL, GetModuleHandle(NULL), NULL);
            
            CreateWindow("STATIC", "Chercher:", WS_CHILD | WS_VISIBLE, 5, 10, 60, 20, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            hSearchEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 70, 7, 150, 25, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            
            CreateWindow("STATIC", "Remplacer:", WS_CHILD | WS_VISIBLE, 230, 10, 70, 20, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            hReplaceEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 305, 7, 150, 25, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            
            hBtnNext = CreateWindow("BUTTON", "Suivant", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 465, 7, 80, 25, hSearchBar, (HMENU)1009, GetModuleHandle(NULL), NULL);
            hBtnReplace = CreateWindow("BUTTON", "Remplacer", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 7, 100, 25, hSearchBar, (HMENU)1010, GetModuleHandle(NULL), NULL);
            hBtnReplaceAll = CreateWindow("BUTTON", "Tous", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 655, 7, 60, 25, hSearchBar, (HMENU)1011, GetModuleHandle(NULL), NULL);
            CreateWindow("BUTTON", "X", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 720, 7, 25, 25, hSearchBar, (HMENU)IDM_EDIT_FIND, GetModuleHandle(NULL), NULL);
            
            // Initialiser NLP
            nlp_init("dictionaries/en.aff", "dictionaries/en.dic");

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
            int searchBarHeight = bShowSearchBar ? 40 : 0;
            
            int tabHeight = 30;
            if (hTabCtrl) {
                SetWindowPos(hTabCtrl, NULL, 0, yEditor, width, tabHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                yEditor += tabHeight;
            }

            if (bShowSearchBar) {
                SetWindowPos(hSearchBar, NULL, 0, yEditor, width, searchBarHeight, SWP_NOZORDER | SWP_NOACTIVATE);
                yEditor += searchBarHeight;
            }

            int hEditorHeight = height - yEditor - sbHeight;
            
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

        case WM_CONTEXTMENU: {
            if ((HWND)wParam == hEditor) {
                HMENU hCtx = CreatePopupMenu();
                AppendMenu(hCtx, MF_STRING, IDM_EDIT_COPY, "Copier");
                AppendMenu(hCtx, MF_STRING, IDM_EDIT_PASTE, "Coller");
                AppendMenu(hCtx, MF_SEPARATOR, 0, NULL);
                AppendMenu(hCtx, MF_STRING, 1030, "Demander à l'IA...");
                
                POINT pt;
                GetCursorPos(&pt);
                TrackPopupMenu(hCtx, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hCtx);
            }
            break;
        }

        case WM_DESTROY:
            if (hbrDarkBackground) DeleteObject(hbrDarkBackground);
            if (g_ruleset) free_ruleset(g_ruleset);
            nlp_cleanup();
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
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "Nouveau\tCtrl+N");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "Ouvrir\tCtrl+O");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "Sauvegarder\tCtrl+S");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "Fichier");

// Menu Édition
HMENU hEditMenu = CreatePopupMenu();
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_COPY, "Copier");
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_PASTE, "Coller");
AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_FIND, "Rechercher...\tCtrl+F");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "Édition");

// Menu Affichage
HMENU hViewMenu = CreatePopupMenu();
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_DARKMODE, "Mode Sombre");
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_SETTINGS, "Paramètres...");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, "Affichage");

// Menu IA (Intégration Dev-C)
HMENU hAIMenu = CreatePopupMenu();
AppendMenu(hAIMenu, MF_STRING, 1020, "Résumé du texte");
AppendMenu(hAIMenu, MF_STRING, 1021, "Améliorer le style");
AppendMenu(hAIMenu, MF_STRING, 1022, "Correction Grammaticale");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hAIMenu, "IA & NLP");

// Attacher la barre de menus à la fenêtre
SetMenu(hwnd, hMenu);


    ShowWindow(hwnd, nCmdShow);

    // Définir les raccourcis clavier (Accelerators)
    ACCEL accels[] = {
        { FCONTROL | FVIRTKEY, 'N', IDM_FILE_NEW },
        { FCONTROL | FVIRTKEY, 'O', IDM_FILE_OPEN },
        { FCONTROL | FVIRTKEY, 'S', IDM_FILE_SAVE },
        { FCONTROL | FVIRTKEY, 'F', IDM_EDIT_FIND }
    };
    HACCEL hAccel = CreateAcceleratorTable(accels, 4);

    // Boucle de messages
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    DestroyAcceleratorTable(hAccel);
    return 0;
}
