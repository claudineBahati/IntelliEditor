#ifndef DIALOGS_H
#define DIALOGS_H

#include <windows.h>
#include <stdbool.h>

bool Dialog_OpenFile(HWND hParent, char* outFilePath, int maxLen);
bool Dialog_SaveFile(HWND hParent, char* outFilePath, int maxLen);
void Dialogs_ShowSettings(HWND hParent);

#endif // DIALOGS_H
