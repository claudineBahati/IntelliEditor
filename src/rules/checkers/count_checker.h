#ifndef COUNT_CHECKER_H
#define COUNT_CHECKER_H

/*
 * Retourne 1 si le texte contient au moins min_words mots, 0 sinon.
 */
int check_word_count_min(const char *text, int min_words);

/*
 * Retourne 1 si le texte contient au plus max_words mots, 0 sinon.
 */
int check_word_count_max(const char *text, int max_words);

/*
 * Compte le nombre de mots dans text.
 */
int count_words(const char *text);

/*
 * Extrait la valeur numérique depuis un paramètre JSON.
 * Supporte :
 *   - "300"
 *   - { "section": "Introduction", "min_words": 300 }
 */
int extract_min_words(const char *parameter);
int extract_max_words(const char *parameter);

#endif /* COUNT_CHECKER_H */
