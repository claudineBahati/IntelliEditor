#ifndef SECTION_CHECKER_H
#define SECTION_CHECKER_H

/*
 * Vérifie que la section dont le titre est *section* est présente dans
 * le document (recherche insensible à la casse).
 * Retourne 1 si trouvé, 0 sinon.
 */
int check_section_exists(const char *document, const char *section);

/*
 * Vérifie que les sections apparaissent dans l'ordre indiqué par
 * *parameter*, une liste de noms séparés par des virgules.
 * ex: parameter = "Résumé,Introduction,Méthodologie,Conclusion"
 * Retourne 1 si l'ordre est respecté (ou si des sections sont absentes
 * mais celles présentes sont dans le bon ordre), 0 sinon.
 */
int check_section_order(const char *document, const char *parameter);

#endif /* SECTION_CHECKER_H */
