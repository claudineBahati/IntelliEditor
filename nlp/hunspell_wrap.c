#include "hunspell_wrap.h"
#include "hunspell_mock.h" // Utilisation du mock car lib absente
#include <stdio.h>

// On crée un pointeur vers l'objet Hunspell (notre moteur de dictionnaire)
static Hunhandle* pHunspell = NULL;

// 1. Initialisation
bool hunspell_wrap_init(const char* aff_path, const char* dic_path) {
    // On crée l'instance du correcteur avec les fichiers .aff et .dic
    pHunspell = Hunspell_create(aff_path, dic_path);
    
    if (pHunspell != NULL) {
        printf("[NLP] Hunspell initialise avec succes.\n");
        return true;
    }
    
    fprintf(stderr, "[NLP] Erreur : Impossible de charger les dictionnaires.\n");
    return false;
}

// 2. Vérification d'un mot
bool hunspell_wrap_check_word(const char* word) {
    if (pHunspell == NULL) return false;
    
    // Hunspell renvoie 0 si le mot est FAUX, et autre chose si c'est JUSTE
    int result = Hunspell_spell(pHunspell, word);
    return (result != 0);
}

// 3. Nettoyage
void hunspell_wrap_cleanup() {
    if (pHunspell != NULL) {
        Hunspell_destroy(pHunspell);
        pHunspell = NULL;
        printf("[NLP] Hunspell libere.\n");
    }
}