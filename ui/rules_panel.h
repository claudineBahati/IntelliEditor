#ifndef RULES_PANEL_H
#define RULES_PANEL_H

#include <windows.h>

HWND RulesPanel_Create(HWND hParent, int x, int y, int width, int height, int id);
void RulesPanel_AddRule(HWND hRulesPanel, const char* ruleText);
void RulesPanel_Clear(HWND hRulesPanel);

#endif // RULES_PANEL_H
