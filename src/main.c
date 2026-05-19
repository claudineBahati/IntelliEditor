#include "editor_core.h"
#include "encoding.h"
#include "tokenizer.h"
#include "exporter.h"
#include "debug_memory.h"
#include "search.h"
#include "gap_buffer.h"
#include "spellcheck.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Pour strcat et strlen

#include "rules/rule_parser.h"
#include "rules/rule_report.h"


#define MAX_DOC_SIZE 4096 // Taille max du document saisi

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // On prépare un buffer pour stocker ce que l'utilisateur va taper
    char document[MAX_DOC_SIZE] = "";
    char line[256];

    printf("=== IntelliEditor - Saisie de document ===\n");
    printf("Entrez votre texte ci-dessous.\n");
    printf("(Tapez 'FIN' sur une nouvelle ligne pour terminer la saisie)\n");
    printf("------------------------------------------\n");

    // Boucle pour récupérer plusieurs lignes
    while (fgets(line, sizeof(line), stdin)) {
        // Si l'utilisateur tape "FIN", on arrête la saisie
        if (strncmp(line, "FIN", 3) == 0) {
            break;
        }

        // On vérifie qu'on ne dépasse pas la capacité du buffer
        if (strlen(document) + strlen(line) < MAX_DOC_SIZE - 1) {
            strcat(document, line);
        } else {
            printf("Attention : Limite de texte atteinte !\n");
            break;
        }
    }

    printf("\n--- Analyse en cours ---\n");

    RuleSet ruleset = {0};

    // Note : N'oublie pas de régler le 'Working Directory' dans CLion
    // sur G:/INTELLI pour que ce chemin fonctionne
    if (!load_rules_from_file(
        "data/rule_templates/memory_license.json",
        &ruleset
    )) {
        printf("Erreur : Impossible de charger le fichier JSON.\n");
        system("pause");
        return 1;
    }

    // On lance l'analyse sur le texte saisi par l'utilisateur
    RuleReport report = evaluate_rules(&ruleset, document);

    // Affichage des résultats
    print_rule_report(&report);

    printf("\nAnalyse terminee avec succes !\n");
    system("pause");
    return 0;
}