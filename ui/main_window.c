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

static void ConvertUtf8ToAnsi(const char* utf8, char* ansi, int ansiSize) {
    if (!utf8 || !ansi || ansiSize <= 0) return;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (wlen > 0) {
        wchar_t* wstr = malloc(wlen * sizeof(wchar_t));
        if (wstr) {
            MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, wlen);
            WideCharToMultiByte(CP_ACP, 0, wstr, -1, ansi, ansiSize, NULL, NULL);
            free(wstr);
            return;
        }
    }
    strncpy(ansi, utf8, ansiSize - 1);
    ansi[ansiSize - 1] = '\0';
}

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

// Déclaration de l'éditeur Scintilla et composants
static HWND hEditor = NULL;
static HWND hEditorContainer = NULL; // Conteneur pour les marges verticales de page
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

// Variables de configuration globales (définitions)
BOOL bDarkMode = FALSE;
int g_pageMargin = 60;
char g_fontName[32] = "Calibri";
int g_fontSize = 12;
int g_pageWidth = 800;
BOOL bShowLineNumbers = FALSE;

static BOOL bShowSearchBar = FALSE;
static HBRUSH hbrDarkBackground = NULL;
static HBRUSH hbrLightBackground = NULL;
static HBRUSH hbrEditorBgDark = NULL;
static HBRUSH hbrEditorBgLight = NULL;

#define MAX_TABS 10
typedef struct {
    sptr_t pDoc;
    char szTitle[64];
    char szFileName[MAX_PATH];
} TabInfo;

static TabInfo g_tabs[MAX_TABS];
static int g_tabCount = 0;
static int g_currentTabIdx = 0;

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

            // Mettre à jour l'onglet actuel
            strcpy(g_tabs[g_currentTabIdx].szFileName, szFileName);
            char* filePart = strrchr(szFileName, '\\');
            if (filePart) {
                filePart++;
            } else {
                filePart = (char*)szFileName;
            }
            strncpy(g_tabs[g_currentTabIdx].szTitle, filePart, sizeof(g_tabs[g_currentTabIdx].szTitle) - 1);
            
            TCITEM tie;
            tie.mask = TCIF_TEXT;
            tie.pszText = g_tabs[g_currentTabIdx].szTitle;
            SendMessage(hTabCtrl, TCM_SETITEM, g_currentTabIdx, (LPARAM)&tie);
        }
        fclose(file);
    }
}

void SwitchToTab(HWND hwnd, int index) {
    if (index >= 0 && index < g_tabCount) {
        g_currentTabIdx = index;
        SendMessage(hEditor, SCI_SETDOCPOINTER, 0, g_tabs[index].pDoc);
        
        // Mettre à jour les règles
        if (g_ruleset) {
            apply_rules(hEditor, g_ruleset);
        }
    }
}

void AddNewTab(HWND hwnd) {
    if (g_tabCount >= MAX_TABS - 1) return; // Garder la place pour le "+"
    
    sptr_t newDoc = SendMessage(hEditor, SCI_CREATEDOCUMENT, 0, 0);
    g_tabs[g_tabCount].pDoc = newDoc;
    snprintf(g_tabs[g_tabCount].szTitle, sizeof(g_tabs[g_tabCount].szTitle), "Sans titre %d", g_tabCount + 1);
    g_tabs[g_tabCount].szFileName[0] = '\0';
    
    // Configurer le nouveau document avec les bons styles
    sptr_t oldDoc = SendMessage(hEditor, SCI_GETDOCPOINTER, 0, 0);
    SendMessage(hEditor, SCI_SETDOCPOINTER, 0, newDoc);
    Scintilla_Configure(hEditor);
    if (bDarkMode) Scintilla_SetTheme(hEditor, TRUE);
    SendMessage(hEditor, SCI_SETDOCPOINTER, 0, oldDoc);

    TCITEM tie;
    tie.mask = TCIF_TEXT;
    tie.pszText = g_tabs[g_tabCount].szTitle;
    
    // Insérer juste avant le "+"
    SendMessage(hTabCtrl, TCM_INSERTITEM, g_tabCount, (LPARAM)&tie);
    
    g_tabCount++;
    
    // Sélectionner l'onglet
    SendMessage(hTabCtrl, TCM_SETCURSEL, g_tabCount - 1, 0);
    SwitchToTab(hwnd, g_tabCount - 1);
}

void CloseTab(HWND hwnd, int index) {
    if (g_tabCount <= 1) {
        MessageBox(hwnd, "Impossible de fermer le dernier onglet.", "IntelliEditor", MB_OK | MB_ICONWARNING);
        return;
    }
    
    // Si on ferme l'onglet actif, on doit basculer sur un autre onglet d'abord
    if (index == g_currentTabIdx) {
        int nextTab = (index > 0) ? index - 1 : index + 1;
        if (nextTab >= g_tabCount) nextTab = 0;
        SendMessage(hTabCtrl, TCM_SETCURSEL, nextTab, 0);
        SwitchToTab(hwnd, nextTab);
    }
    
    // Libérer le document Scintilla
    SendMessage(hEditor, SCI_RELEASEDOCUMENT, 0, g_tabs[index].pDoc);
    
    // Décaler les onglets restants
    for (int i = index; i < g_tabCount - 1; i++) {
        g_tabs[i] = g_tabs[i + 1];
    }
    g_tabCount--;
    
    // Ajuster l'index de l'onglet courant
    if (g_currentTabIdx > index) {
        g_currentTabIdx--;
    } else if (g_currentTabIdx == g_tabCount) {
        g_currentTabIdx = g_tabCount - 1;
    }
    
    // Supprimer l'onglet du Tab Control
    SendMessage(hTabCtrl, TCM_DELETEITEM, index, 0);
    
    // S'assurer que le bon onglet est visuellement sélectionné
    SendMessage(hTabCtrl, TCM_SETCURSEL, g_currentTabIdx, 0);
    SwitchToTab(hwnd, g_currentTabIdx);
}

void InitTabSystem(HWND hwnd) {
    sptr_t firstDoc = SendMessage(hEditor, SCI_GETDOCPOINTER, 0, 0);
    SendMessage(hEditor, SCI_ADDREFDOCUMENT, 0, firstDoc);
    g_tabs[0].pDoc = firstDoc;
    strcpy(g_tabs[0].szTitle, "Sans titre 1");
    g_tabs[0].szFileName[0] = '\0';
    g_tabCount = 1;
    g_currentTabIdx = 0;
}

// Fonction qui gère les messages de la fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hbr = bDarkMode ? hbrDarkBackground : hbrLightBackground;
            if (hbr) {
                FillRect(hdc, &rect, hbr);
                return 1;
            }
            break;
        }
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;
            // Si c'est le conteneur de l'éditeur Scintilla, appliquer le même fond pour simuler la page
            if (hwndStatic == hEditorContainer) {
                HBRUSH hbr = bDarkMode ? hbrEditorBgDark : hbrEditorBgLight;
                SetBkColor(hdc, bDarkMode ? RGB(28, 30, 38) : RGB(255, 255, 255));
                return (LRESULT)hbr;
            }
            if (bDarkMode) {
                SetTextColor(hdc, RGB(220, 220, 220));
                SetBkColor(hdc, RGB(45, 45, 45));
                return (LRESULT)hbrDarkBackground;
            }
            break;
        }
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
                    AddNewTab(hwnd);
                    break;
                case IDM_FILE_OPEN: {
                    char szFileName[MAX_PATH] = {0};
                    if (Dialog_OpenFile(hwnd, szFileName, MAX_PATH)) {
                        int length = Scintilla_GetTextLength(hEditor);
                        if (length == 0 && strlen(g_tabs[g_currentTabIdx].szFileName) == 0) {
                            LoadFileToEditor(hwnd, szFileName);
                        } else {
                            if (g_tabCount < MAX_TABS - 1) {
                                // 1. Créer le document
                                sptr_t newDoc = SendMessage(hEditor, SCI_CREATEDOCUMENT, 0, 0);
                                // 2. L'ajouter à nos structures
                                g_tabs[g_tabCount].pDoc = newDoc;
                                g_tabs[g_tabCount].szFileName[0] = '\0';
                                strcpy(g_tabs[g_tabCount].szTitle, "Chargement...");
                                
                                // 3. Insérer le nouvel onglet avant le "+"
                                TCITEM tie;
                                tie.mask = TCIF_TEXT;
                                tie.pszText = g_tabs[g_tabCount].szTitle;
                                SendMessage(hTabCtrl, TCM_INSERTITEM, g_tabCount, (LPARAM)&tie);
                                
                                // 4. Incrémenter le nombre d'onglets
                                g_tabCount++;
                                
                                // 5. Sélectionner et basculer sur le nouvel onglet
                                SendMessage(hTabCtrl, TCM_SETCURSEL, g_tabCount - 1, 0);
                                SwitchToTab(hwnd, g_tabCount - 1);
                                
                                // 6. Configurer et charger le fichier dans le document actif
                                Scintilla_Configure(hEditor);
                                if (bDarkMode) Scintilla_SetTheme(hEditor, TRUE);
                                LoadFileToEditor(hwnd, szFileName);
                            }
                        }
                    }
                    break;
                }
                case IDM_FILE_SAVE: {
                    char szFileName[MAX_PATH] = {0};
                    BOOL doSave = FALSE;
                    if (strlen(g_tabs[g_currentTabIdx].szFileName) > 0) {
                        strcpy(szFileName, g_tabs[g_currentTabIdx].szFileName);
                        doSave = TRUE;
                    } else {
                        if (Dialog_SaveFile(hwnd, szFileName, MAX_PATH)) {
                            strcpy(g_tabs[g_currentTabIdx].szFileName, szFileName);
                            char* filePart = strrchr(szFileName, '\\');
                            if (filePart) {
                                filePart++;
                            } else {
                                filePart = szFileName;
                            }
                            strncpy(g_tabs[g_currentTabIdx].szTitle, filePart, sizeof(g_tabs[g_currentTabIdx].szTitle) - 1);
                            TCITEM tie;
                            tie.mask = TCIF_TEXT;
                            tie.pszText = g_tabs[g_currentTabIdx].szTitle;
                            SendMessage(hTabCtrl, TCM_SETITEM, g_currentTabIdx, (LPARAM)&tie);
                            doSave = TRUE;
                        }
                    }
                    if (doSave) {
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
                case SC_CLOSE:
                    DestroyWindow(hwnd);
                    break;

                // --- ÉDITION EXTENSION ---
                case IDM_EDIT_UNDO:
                    SendMessage(hEditor, SCI_UNDO, 0, 0);
                    break;
                case IDM_EDIT_REDO:
                    SendMessage(hEditor, SCI_REDO, 0, 0);
                    break;
                case IDM_EDIT_COPY:
                    Scintilla_Copy(hEditor);
                    break;
                case IDM_EDIT_PASTE:
                    Scintilla_Paste(hEditor);
                    break;
                case IDM_EDIT_SELECTALL:
                    SendMessage(hEditor, SCI_SELECTALL, 0, 0);
                    break;
                case IDM_EDIT_FIND:
                    bShowSearchBar = !bShowSearchBar;
                    ShowWindow(hSearchBar, bShowSearchBar ? SW_SHOW : SW_HIDE);
                    // Déclencher un redimensionnement
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
                    if (bShowSearchBar) SetFocus(hSearchEdit);
                    break;

                case 1009: { // Bouton 'Suivant'
                    char szSearch[256];
                    GetWindowText(hSearchEdit, szSearch, sizeof(szSearch));
                    if (strlen(szSearch) > 0) {
                        SendMessage(hEditor, SCI_SEARCHANCHOR, 0, 0);
                        int pos = (int)SendMessage(hEditor, SCI_SEARCHNEXT, SCFIND_NONE, (LPARAM)szSearch);
                        if (pos != -1) {
                            SendMessage(hEditor, SCI_SCROLLCARET, 0, 0);
                        } else {
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
                        SendMessage(hwnd, WM_COMMAND, 1009, 0);
                    }
                    break;
                }
                case 1011: { // Bouton 'Tous'
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

                // --- INSERTION EXTENSION ---
                case IDM_INSERT_PAGEBREAK:
                    SendMessage(hEditor, SCI_REPLACESEL, 0, (LPARAM)"\n\f\n");
                    break;
                case IDM_INSERT_DATETIME: {
                    SYSTEMTIME lt;
                    GetLocalTime(&lt);
                    char szDate[128];
                    snprintf(szDate, sizeof(szDate), "%02d/%02d/%04d %02d:%02d", lt.wDay, lt.wMonth, lt.wYear, lt.wHour, lt.wMinute);
                    SendMessage(hEditor, SCI_REPLACESEL, 0, (LPARAM)szDate);
                    break;
                }

                // --- MISE EN PAGE EXTENSION ---
                case IDM_LAYOUT_MARGIN_NARROW:
                    g_pageMargin = 20;
                    Scintilla_Configure(hEditor);
                    SendMessage(hwnd, WM_SIZE, 0, lParam);
                    break;
                case IDM_LAYOUT_MARGIN_NORMAL:
                    g_pageMargin = 60;
                    Scintilla_Configure(hEditor);
                    SendMessage(hwnd, WM_SIZE, 0, lParam);
                    break;
                case IDM_LAYOUT_MARGIN_WIDE:
                    g_pageMargin = 100;
                    Scintilla_Configure(hEditor);
                    SendMessage(hwnd, WM_SIZE, 0, lParam);
                    break;
                case IDM_LAYOUT_ORIENT_PORTRAIT:
                    g_pageWidth = 800;
                    SendMessage(hwnd, WM_SIZE, 0, lParam);
                    break;
                case IDM_LAYOUT_ORIENT_LANDSCAPE:
                    g_pageWidth = 1100;
                    SendMessage(hwnd, WM_SIZE, 0, lParam);
                    break;

                // --- REVISION & IA EXTENSION ---
                case IDM_REVIEW_STATS: {
                    int charCount = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
                    int wordCount = Scintilla_GetWordCount(hEditor);
                    int lineCount = (int)SendMessage(hEditor, SCI_GETLINECOUNT, 0, 0);
                    char msg[256];
                    snprintf(msg, sizeof(msg), "Statistiques du document :\n\n- Caracteres : %d\n- Mots : %d\n- Lignes : %d", charCount, wordCount, lineCount);
                    MessageBox(hwnd, msg, "Statistiques", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                case IDM_REVIEW_SUMMARY: {
                    int len = Scintilla_GetTextLength(hEditor);
                    if (len > 0) {
                        char* txt = malloc(len + 1);
                        Scintilla_GetText(hEditor, txt, len + 1);
                        char summary[300] = {0};
                        if (len < 250) {
                            strcpy(summary, txt);
                        } else {
                            strncpy(summary, txt, 247);
                            strcat(summary, "...");
                        }
                        char msg[512];
                        snprintf(msg, sizeof(msg), "Resume automatique (Extrait principal) :\n\n%s", summary);
                        MessageBox(hwnd, msg, "Resume de Texte IA", MB_OK | MB_ICONINFORMATION);
                        free(txt);
                    } else {
                        MessageBox(hwnd, "Le document est vide.", "Resume de Texte IA", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                case IDM_REVIEW_STYLE: {
                    int startSel = (int)SendMessage(hEditor, SCI_GETSELECTIONSTART, 0, 0);
                    int endSel = (int)SendMessage(hEditor, SCI_GETSELECTIONEND, 0, 0);
                    int selLen = endSel - startSel;
                    if (selLen > 0) {
                        char* selText = malloc(selLen + 1);
                        SendMessage(hEditor, SCI_GETSELTEXT, 0, (LPARAM)selText);
                        ParaphraseReport pReport = nlp_generate_paraphrase(selText);
                        if (pReport.suggestion_count > 0) {
                            char msg[1024];
                            snprintf(msg, sizeof(msg), "Suggestion de style IA pour :\n\"%s\"\n\nNouvelle version :\n\"%s\"\n\nSouhaitez-vous remplacer le texte ?", selText, pReport.suggestions[0]);
                            int res = MessageBox(hwnd, msg, "Ameliorer le style (IA)", MB_YESNO | MB_ICONQUESTION);
                            if (res == IDYES) {
                                SendMessage(hEditor, SCI_REPLACESEL, 0, (LPARAM)pReport.suggestions[0]);
                            }
                        } else {
                            MessageBox(hwnd, "Aucune suggestion de style disponible.", "Ameliorer le style (IA)", MB_OK | MB_ICONINFORMATION);
                        }
                        free(selText);
                    } else {
                        MessageBox(hwnd, "Veuillez selectionner le texte a ameliorer.", "Ameliorer le style (IA)", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                case IDM_REVIEW_SPELLCHECK: {
                    int len = Scintilla_GetTextLength(hEditor);
                    if (len > 0) {
                        char* txt = malloc(len + 1);
                        Scintilla_GetText(hEditor, txt, len + 1);
                        CorrectionReport report = nlp_process_text(txt);
                        char msg[512];
                        snprintf(msg, sizeof(msg), "Analyse orthographe & grammaire :\n\n- Mots mal orthographies : %d\n- Erreurs de grammaire : %d", report.sp_error_count, report.gr_error_count);
                        MessageBox(hwnd, msg, "Correcteur orthographe", MB_OK | MB_ICONINFORMATION);
                        nlp_free_report(&report);
                        free(txt);
                    } else {
                        MessageBox(hwnd, "Aucun texte a analyser.", "Correcteur orthographe", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }

                // --- AFFICHAGE EXTENSION ---
                case IDM_VIEW_DARKMODE:
                    bDarkMode = !bDarkMode;
                    Scintilla_SetTheme(hEditor, bDarkMode);
                    Toolbar_SetDarkMode(hToolbar, bDarkMode);
                    Statusbar_SetDarkMode(hStatusbar, bDarkMode);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_DARKMODE, bDarkMode ? MF_CHECKED : MF_UNCHECKED);
                    
                    InvalidateRect(hwnd, NULL, TRUE);
                    InvalidateRect(hToolbar, NULL, TRUE);
                    InvalidateRect(hStatusbar, NULL, TRUE);
                    if (hEditorContainer) InvalidateRect(hEditorContainer, NULL, TRUE);
                    InvalidateRect(hRulesPanel, NULL, TRUE);
                    break;

                case IDM_VIEW_LINENUMBERS:
                    bShowLineNumbers = !bShowLineNumbers;
                    Scintilla_Configure(hEditor);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_LINENUMBERS, bShowLineNumbers ? MF_CHECKED : MF_UNCHECKED);
                    break;

                case IDM_VIEW_SETTINGS:
                    Dialogs_ShowSettings(hwnd);
                    break;

                case ID_RULES_PANEL: {
                    if (HIWORD(wParam) == LBN_SELCHANGE) {
                        int index = (int)SendMessage(hRulesPanel, LB_GETCURSEL, 0, 0);
                        if (index != LB_ERR && g_ruleset && index < g_ruleset->count) {
                            Rule* r = &g_ruleset->rules[index];
                            if (strlen(r->parameter) > 0) {
                                SetWindowText(hSearchEdit, r->parameter);
                                SendMessage(hwnd, WM_COMMAND, 1009, 0);
                            }
                        }
                    }
                    break;
                }

                case 3002: { // Application dynamique des nouveaux reglages
                    Scintilla_Configure(hEditor);
                    Scintilla_SetTheme(hEditor, bDarkMode);
                    Toolbar_SetDarkMode(hToolbar, bDarkMode);
                    Statusbar_SetDarkMode(hStatusbar, bDarkMode);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_DARKMODE, bDarkMode ? MF_CHECKED : MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_VIEW_LINENUMBERS, bShowLineNumbers ? MF_CHECKED : MF_UNCHECKED);
                    
                    // Forcer un redimensionnement
                    RECT rc;
                    GetClientRect(hwnd, &rc);
                    SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
                    
                    InvalidateRect(hwnd, NULL, TRUE);
                    InvalidateRect(hToolbar, NULL, TRUE);
                    InvalidateRect(hStatusbar, NULL, TRUE);
                    if (hEditorContainer) InvalidateRect(hEditorContainer, NULL, TRUE);
                    InvalidateRect(hRulesPanel, NULL, TRUE);
                    break;
                }
            }
            break;
        }
        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            char szFileName[MAX_PATH];
            if (DragQueryFile(hDrop, 0, szFileName, MAX_PATH)) {
                if (strstr(szFileName, ".json")) {
                    RuleSet* new_rules = load_rules(szFileName);
                    if (new_rules) {
                        if (g_ruleset) free_ruleset(g_ruleset);
                        g_ruleset = new_rules;
                        RulesPanel_Clear(hRulesPanel);
                        for (int i = 0; i < g_ruleset->count; i++) {
                            char buffer[256];
                            char ansiBuf[256];
                            snprintf(buffer, sizeof(buffer), "[%s] %s", g_ruleset->rules[i].id, g_ruleset->rules[i].description);
                            ConvertUtf8ToAnsi(buffer, ansiBuf, sizeof(ansiBuf));
                            RulesPanel_AddRule(hRulesPanel, ansiBuf);
                        }
                        apply_rules(hEditor, g_ruleset);
                        MessageBox(hwnd, "Nouvelles regles chargees avec succes !", "Regles metier", MB_OK | MB_ICONINFORMATION);
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
            
            // Gestion du changement d'onglet
            if (phdr->hwndFrom == hTabCtrl && phdr->code == TCN_SELCHANGE) {
                int sel = (int)SendMessage(hTabCtrl, TCM_GETCURSEL, 0, 0);
                if (sel == g_tabCount) { // Clic sur le "+"
                    AddNewTab(hwnd);
                } else {
                    SwitchToTab(hwnd, sel);
                }
            }

            // --- FERMETURE D'ONGLET PAR CLIC DROIT ---
            if (phdr->hwndFrom == hTabCtrl && phdr->code == NM_RCLICK) {
                TCHITTESTINFO hti;
                GetCursorPos(&hti.pt);
                ScreenToClient(hTabCtrl, &hti.pt);
                int clickedTab = (int)SendMessage(hTabCtrl, TCM_HITTEST, 0, (LPARAM)&hti);
                if (clickedTab >= 0 && clickedTab < g_tabCount) {
                    HMENU hTabMenu = CreatePopupMenu();
                    AppendMenu(hTabMenu, MF_STRING, 3001, "Fermer l'onglet");
                    
                    POINT pt;
                    GetCursorPos(&pt);
                    int cmd = TrackPopupMenu(hTabMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    DestroyMenu(hTabMenu);
                    
                    if (cmd == 3001) {
                        CloseTab(hwnd, clickedTab);
                    }
                }
            }
            
            // Custom Draw pour le look Premium (Toolbar et Statusbar)
            if (bDarkMode && (phdr->code == NM_CUSTOMDRAW)) {
                LPNMCUSTOMDRAW lpnmcd = (LPNMCUSTOMDRAW)lParam;
                if (phdr->hwndFrom == hToolbar || phdr->hwndFrom == hStatusbar) {
                    if (lpnmcd->dwDrawStage == CDDS_PREPAINT) {
                        FillRect(lpnmcd->hdc, &lpnmcd->rc, hbrDarkBackground);
                        return CDRF_NOTIFYPOSTPAINT;
                    }
                    if (lpnmcd->dwDrawStage == CDDS_ITEMPREPAINT) {
                        SetTextColor(lpnmcd->hdc, RGB(220, 220, 225));
                        SetBkMode(lpnmcd->hdc, TRANSPARENT);
                        return CDRF_DODEFAULT;
                    }
                }
            }
            break;
        }
        case WM_CREATE: {
            hToolbar = Toolbar_Create(hwnd);
            hStatusbar = Statusbar_Create(hwnd);

            // Pinceaux
            hbrDarkBackground = CreateSolidBrush(RGB(45, 45, 45));
            hbrLightBackground = CreateSolidBrush(RGB(220, 224, 230));
            hbrEditorBgDark = CreateSolidBrush(RGB(28, 30, 38));
            hbrEditorBgLight = CreateSolidBrush(RGB(255, 255, 255));

            hRulesPanel = RulesPanel_Create(hwnd, 0, 0, 0, 0, ID_RULES_PANEL);

            // Conteneur de page (marges)
            hEditorContainer = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)2007, GetModuleHandle(NULL), NULL);

            // Créer Scintilla avec le conteneur comme parent
            hEditor = Scintilla_Create(hEditorContainer, 0, 0, 0, 0, ID_EDITOR);

            g_ruleset = load_rules("data/rules.json");
            if (g_ruleset) {
                RulesPanel_Clear(hRulesPanel);
                for (int i = 0; i < g_ruleset->count; i++) {
                    char buffer[256];
                    char ansiBuf[256];
                    snprintf(buffer, sizeof(buffer), "[%s] %s", g_ruleset->rules[i].id, g_ruleset->rules[i].description);
                    ConvertUtf8ToAnsi(buffer, ansiBuf, sizeof(ansiBuf));
                    RulesPanel_AddRule(hRulesPanel, ansiBuf);
                }
                apply_rules(hEditor, g_ruleset);
            }

            hTabCtrl = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)2006, GetModuleHandle(NULL), NULL);
            HFONT hTabFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
            SendMessage(hTabCtrl, WM_SETFONT, (WPARAM)hTabFont, TRUE);

            InitTabSystem(hwnd);

            TCITEM tie;
            tie.mask = TCIF_TEXT;
            tie.pszText = g_tabs[0].szTitle;
            SendMessage(hTabCtrl, TCM_INSERTITEM, 0, (LPARAM)&tie);
            tie.pszText = "+";
            SendMessage(hTabCtrl, TCM_INSERTITEM, 1, (LPARAM)&tie);

            hSearchBar = CreateWindowEx(0, "STATIC", "", WS_CHILD | WS_BORDER, 0, 0, 0, 0, hwnd, NULL, GetModuleHandle(NULL), NULL);
            
            CreateWindow("STATIC", "Chercher:", WS_CHILD | WS_VISIBLE, 5, 10, 60, 20, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            hSearchEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 70, 7, 150, 25, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            
            CreateWindow("STATIC", "Remplacer:", WS_CHILD | WS_VISIBLE, 230, 10, 70, 20, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            hReplaceEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 305, 7, 150, 25, hSearchBar, NULL, GetModuleHandle(NULL), NULL);
            
            hBtnNext = CreateWindow("BUTTON", "Suivant", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 465, 7, 80, 25, hSearchBar, (HMENU)1009, GetModuleHandle(NULL), NULL);
            hBtnReplace = CreateWindow("BUTTON", "Remplacer", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 7, 100, 25, hSearchBar, (HMENU)1010, GetModuleHandle(NULL), NULL);
            hBtnReplaceAll = CreateWindow("BUTTON", "Tous", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 655, 7, 60, 25, hSearchBar, (HMENU)1011, GetModuleHandle(NULL), NULL);
            CreateWindow("BUTTON", "X", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 720, 7, 25, 25, hSearchBar, (HMENU)IDM_EDIT_FIND, GetModuleHandle(NULL), NULL);
            
            nlp_init("dictionaries/en.aff", "dictionaries/en.dic");
            DragAcceptFiles(hwnd, TRUE);
            break;
        }

        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
            RECT rcToolbar;
            GetWindowRect(hToolbar, &rcToolbar);
            int tbHeight = rcToolbar.bottom - rcToolbar.top;

            SendMessage(hStatusbar, WM_SIZE, 0, 0);
            RECT rcStatus;
            GetWindowRect(hStatusbar, &rcStatus);
            int sbHeight = rcStatus.bottom - rcStatus.top;

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
            int rulesWidth = 250;
            int editorAreaWidth = width - rulesWidth;

            int pageRealWidth = g_pageWidth;
            int xEditor = 0;
            int wEditor = editorAreaWidth;
            if (editorAreaWidth > pageRealWidth + 40) {
                xEditor = (editorAreaWidth - pageRealWidth) / 2;
                wEditor = pageRealWidth;
            }

            // Positionner hEditorContainer et hEditor
            if (hEditorContainer) {
                SetWindowPos(hEditorContainer, NULL, xEditor, yEditor + 10, wEditor, hEditorHeight - 20, SWP_NOZORDER | SWP_NOACTIVATE);
                if (hEditor) {
                    SetWindowPos(hEditor, NULL, 0, 40, wEditor, hEditorHeight - 20 - 80, SWP_NOZORDER | SWP_NOACTIVATE);
                }
            }
            if (hRulesPanel) {
                SetWindowPos(hRulesPanel, NULL, editorAreaWidth, yEditor, rulesWidth, hEditorHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }

        case WM_DESTROY:
            if (hbrDarkBackground) DeleteObject(hbrDarkBackground);
            if (hbrLightBackground) DeleteObject(hbrLightBackground);
            if (hbrEditorBgDark) DeleteObject(hbrEditorBgDark);
            if (hbrEditorBgLight) DeleteObject(hbrEditorBgLight);
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
// Créer la barre de menus (Word-like Ribbon/Tabs)
HMENU hMenu = CreateMenu();

// Menu Fichier
HMENU hFileMenu = CreatePopupMenu();
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "Nouveau\tCtrl+N");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "Ouvrir\tCtrl+O");
AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "Sauvegarder\tCtrl+S");
AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
AppendMenu(hFileMenu, MF_STRING, SC_CLOSE, "Quitter");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "Fichier");

// Menu Edition
HMENU hEditMenu = CreatePopupMenu();
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_UNDO, "Annuler\tCtrl+Z");
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_REDO, "Retablir\tCtrl+Y");
AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_COPY, "Copier\tCtrl+C");
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_PASTE, "Coller\tCtrl+V");
AppendMenu(hEditMenu, MF_SEPARATOR, 0, NULL);
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_SELECTALL, "Tout selectionner\tCtrl+A");
AppendMenu(hEditMenu, MF_STRING, IDM_EDIT_FIND, "Rechercher...\tCtrl+F");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "Edition");

// Menu Insertion
HMENU hInsertMenu = CreatePopupMenu();
AppendMenu(hInsertMenu, MF_STRING, IDM_INSERT_PAGEBREAK, "Saut de page\tCtrl+Entree");
AppendMenu(hInsertMenu, MF_STRING, IDM_INSERT_DATETIME, "Date et Heure");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hInsertMenu, "Insertion");

// Menu Mise en page
HMENU hLayoutMenu = CreatePopupMenu();
AppendMenu(hLayoutMenu, MF_STRING, IDM_LAYOUT_MARGIN_NARROW, "Marges Etroites (20px)");
AppendMenu(hLayoutMenu, MF_STRING, IDM_LAYOUT_MARGIN_NORMAL, "Marges Normales (60px)");
AppendMenu(hLayoutMenu, MF_STRING, IDM_LAYOUT_MARGIN_WIDE, "Marges Larges (100px)");
AppendMenu(hLayoutMenu, MF_SEPARATOR, 0, NULL);
AppendMenu(hLayoutMenu, MF_STRING, IDM_LAYOUT_ORIENT_PORTRAIT, "Orientation Portrait (800px)");
AppendMenu(hLayoutMenu, MF_STRING, IDM_LAYOUT_ORIENT_LANDSCAPE, "Orientation Paysage (1100px)");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hLayoutMenu, "Mise en page");

// Menu Revision & IA
HMENU hReviewMenu = CreatePopupMenu();
AppendMenu(hReviewMenu, MF_STRING, IDM_REVIEW_STATS, "Statistiques de mots");
AppendMenu(hReviewMenu, MF_STRING, IDM_REVIEW_SUMMARY, "Resume du texte (IA)");
AppendMenu(hReviewMenu, MF_STRING, IDM_REVIEW_STYLE, "Ameliorer le style (IA)");
AppendMenu(hReviewMenu, MF_STRING, IDM_REVIEW_SPELLCHECK, "Verification orthographe");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hReviewMenu, "Revision & IA");

// Menu Affichage
HMENU hViewMenu = CreatePopupMenu();
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_DARKMODE, "Mode Sombre");
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_LINENUMBERS, "Afficher les numeros de lignes");
AppendMenu(hViewMenu, MF_STRING, IDM_VIEW_SETTINGS, "Parametres...");
AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, "Affichage");

// Attacher la barre de menus à la fenêtre
SetMenu(hwnd, hMenu);


    ShowWindow(hwnd, nCmdShow);

    // Définir les raccourcis clavier (Accelerators)
    ACCEL accels[] = {
        { FCONTROL | FVIRTKEY, 'N', IDM_FILE_NEW },
        { FCONTROL | FVIRTKEY, 'O', IDM_FILE_OPEN },
        { FCONTROL | FVIRTKEY, 'S', IDM_FILE_SAVE },
        { FCONTROL | FVIRTKEY, 'F', IDM_EDIT_FIND },
        { FCONTROL | FVIRTKEY, 'Z', IDM_EDIT_UNDO },
        { FCONTROL | FVIRTKEY, 'Y', IDM_EDIT_REDO },
        { FCONTROL | FVIRTKEY, 'A', IDM_EDIT_SELECTALL },
        { FCONTROL | FVIRTKEY, VK_RETURN, IDM_INSERT_PAGEBREAK }
    };
    HACCEL hAccel = CreateAcceleratorTable(accels, 8);

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
