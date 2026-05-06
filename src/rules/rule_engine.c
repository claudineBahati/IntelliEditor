#include "rule_parser.h"
#include <Scintilla.h>
#include <string.h>

void apply_rules(HWND hEditor, RuleSet* ruleset) {
    if (!hEditor || !ruleset) return;

    // 1. Nettoyer les anciens indicateurs (on utilise l'indicateur 0 par défaut)
    int length = (int)SendMessage(hEditor, SCI_GETTEXTLENGTH, 0, 0);
    SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
    SendMessage(hEditor, SCI_INDICATORCLEARRANGE, 0, length);

    // Configurer l'apparence de l'indicateur (ex: souligné vague rouge)
    SendMessage(hEditor, SCI_INDICSETSTYLE, 0, INDIC_SQUIGGLE);
    SendMessage(hEditor, SCI_INDICSETFORE, 0, RGB(255, 0, 0));

    // 2. Parcourir les règles
    for (int i = 0; i < ruleset->count; i++) {
        Rule* r = &ruleset->rules[i];
        
        if (strcmp(r->check_type, "forbidden_word") == 0 || strcmp(r->check_type, "regex") == 0) {
            const char* pattern = r->parameter_str;
            if (!pattern) continue;

            int searchFlags = SCFIND_NONE;
            if (strcmp(r->check_type, "regex") == 0) {
                searchFlags |= SCFIND_REGEXP;
            }

            // Recherche et marquage
            int startPos = 0;
            while (startPos < length) {
                SendMessage(hEditor, SCI_SETTARGETSTART, startPos, 0);
                SendMessage(hEditor, SCI_SETTARGETEND, length, 0);
                SendMessage(hEditor, SCI_SETSEARCHFLAGS, searchFlags, 0);
                
                int foundPos = (int)SendMessage(hEditor, SCI_SEARCHINTARGET, strlen(pattern), (LPARAM)pattern);
                if (foundPos == -1) break;

                int endPos = (int)SendMessage(hEditor, SCI_GETTARGETEND, 0, 0);
                
                // Appliquer l'indicateur
                SendMessage(hEditor, SCI_SETINDICATORCURRENT, 0, 0);
                SendMessage(hEditor, SCI_INDICATORFILLRANGE, foundPos, endPos - foundPos);

                startPos = endPos;
            }
        }
    }
}
