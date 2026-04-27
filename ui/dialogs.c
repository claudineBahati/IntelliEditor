#include "dialogs.h"

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
