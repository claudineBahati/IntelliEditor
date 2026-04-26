#include <windows.h>
#include "encoding.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char* utf8_text = "IntelliEditor : Encodage UTF-8 vers UTF-16 OK ! (Test éàç)";
    
    // Convert to UTF-16
    wchar_t* utf16_text = utf8_to_utf16(utf8_text);
    
    // We use MessageBoxW to explicitly call the wide-character (UTF-16) version
    if (utf16_text) {
        MessageBoxW(NULL, utf16_text, L"Test Phase 1 - Encodage", MB_OK | MB_ICONINFORMATION);
        free(utf16_text);
    } else {
        MessageBoxA(NULL, "Erreur d'encodage.", "Erreur", MB_OK | MB_ICONERROR);
    }
    
    return 0;
}
