<<<<<<< HEAD



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/rules/rule_engine.h"
#include "src/rules/rule_parser.h"
#include "src/rules/rule_report.h"


/* Lecture d'un fichier texte entier en mémoire  */

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Impossible d'ouvrir '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0) {
        fclose(f);
        return calloc(1, 1);
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, (size_t)size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}


/* Document de test interne    */

static const char *DEMO_DOCUMENT =
    "# INTRODUCTION\n\n"
    "Résumé\n"
    "Ce mémoire présente une étude approfondie sur l'optimisation des "
    "systèmes embarqués à faible consommation. L'Université Don Bosco de "
    "Lubumbashi (UDBL) encadre ce travail de recherche. La question "
    "centrale est : comment réduire la consommation énergétique des "
    "microcontrôleurs dans des environnements à ressources limitées ?\n\n"
    "Introduction\n"
    "Les systèmes embarqués sont aujourd'hui omniprésents dans notre "
    "quotidien. Des montres connectées aux véhicules autonomes, en passant "
    "par les équipements médicaux, ils jouent un rôle fondamental. "
    "Cependant, leur alimentation en énergie reste un défi majeur. "
    "Ce mémoire propose des techniques nouvelles pour y répondre.\n\n"
    "Revue de littérature\n"
    "De nombreux travaux ont abordé cette problématique. Smith (2018) "
    "propose une architecture à modes de veille adaptatifs. Karim (2020) "
    "explore les compilateurs spécialisés pour réduire les cycles CPU.\n\n"
    "Méthodologie\n"
    "Notre approche repose sur une série d'expériences contrôlées menées "
    "sur trois plateformes matérielles différentes (STM32, ESP32, RISC-V).\n\n"
    "Résultats\n"
    "Les expériences montrent une réduction moyenne de 34% de la "
    "consommation par rapport aux implémentations de référence.\n\n"
    "Discussion\n"
    "Ces résultats confirment l'hypothèse initiale. Les gains sont plus "
    "importants sur les architectures ARM que RISC-V dans nos conditions.\n\n"
    "Conclusion\n"
    "Ce travail ouvre des perspectives prometteuses pour la conception de "
    "systèmes embarqués durables. Des recherches futures pourront étendre "
    "ces résultats à d'autres familles de microcontrôleurs.\n\n"
    "Bibliographie\n"
    "[1] Smith, J. (2018). Low-power embedded design. IEEE Press.\n"
    "[2] Karim, A. (2020). Compiler optimizations for MCU. ACM Trans.\n";


/* main          */

int main(int argc, char *argv[]) {
    printf("=== IntelliEditor — Moteur de Règles (DEV-D) ===\n\n");

    if (argc < 2) {
        printf("Usage : %s <regles.json> [document.txt]\n\n", argv[0]);
        printf("Lancement avec document de démonstration interne…\n");

        /* Charge un fichier de règles par défaut si présent */
        RuleSet   ruleset = {0};
        const char *default_rules =
            "data/rule_templates/memory_license.json";

        if (!load_rules_from_file(default_rules, &ruleset)) {
            fprintf(stderr,
                    "Fichier de règles '%s' introuvable.\n"
                    "Fournissez le chemin en argument.\n",
                    default_rules);
            return 1;
        }

        print_ruleset(&ruleset);
        RuleReport report = evaluate_rules(&ruleset, DEMO_DOCUMENT);
        print_rule_report(&report);
        return 0;
    }

    /* ── Charge les règles ────────────────────────────────────────── */
    RuleSet ruleset = {0};
    if (!load_rules_from_file(argv[1], &ruleset)) {
        fprintf(stderr, "Échec du chargement des règles depuis '%s'.\n",
                argv[1]);
        return 1;
    }
    printf("Fichier de règles : %s\n", argv[1]);
    print_ruleset(&ruleset);

    /* ── Charge ou utilise le document ───────────────────────────── */
    const char *document = DEMO_DOCUMENT;
    char       *file_buf = NULL;

    if (argc >= 3) {
        file_buf = read_file(argv[2]);
        if (!file_buf) {
            fprintf(stderr, "Impossible de lire le document '%s'.\n",
                    argv[2]);
            return 1;
        }
        document = file_buf;
        printf("Document : %s\n", argv[2]);
    } else {
        printf("Document : (document de démonstration interne)\n");
    }

    /* ── Évalue et affiche ────────────────────────────────────────── */
    RuleReport report = evaluate_rules(&ruleset, document);
    print_rule_report(&report);
    system("pause");
    free(file_buf);

    return 0;
}
=======
#include <stdio.h>
#include <windows.h>
#include "llm/llm_thread.h"

int main() {
    printf("=== TEST DE LA FILE D'ATTENTE (QUEUE) ===\n");

    // 1. Initialisation
    llm_config_t config = {"models/llama-1b.gguf", 4};
    if (!llm_thread_init(config)) {
        printf("Erreur d'initialisation !\n");
        return 1;
    }

    // 2. On envoie 3 requêtes très vite
    printf("[Main] Envoi de 3 requetes dans la file...\n");
    llm_thread_query("Quelle est la capitale de la France ?");
    llm_thread_query("Combien font 2 + 2 ?");
    llm_thread_query("Qui est Albert Einstein ?");

    // 3. Boucle de surveillance
    // On attend tant que la file n'est pas vide
    while (llm_thread_is_busy()) {
        printf("[Main] L'interface est FLUIDE. L'IA travaille en arriere-plan...\n");
        Sleep(1500); // On attend 1.5 seconde entre chaque vérification
    }

    printf("\n[Main] Toutes les requetes ont ete traitees.\n");

    // 4. Nettoyage
    llm_thread_stop();
    printf("Programme termine proprement.\n");

    return 0;
}
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
