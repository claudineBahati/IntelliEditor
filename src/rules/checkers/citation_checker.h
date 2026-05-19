#ifndef CITATION_CHECKER_H
#define CITATION_CHECKER_H

/*
 * Vérifie la présence d'une section bibliographique dans le document.
 *
 * *parameter* peut être :
 *   - un nom de section       : "Bibliographie"
 *   - une liste séparée par | : "Bibliographie|Références|Bibliography"
 *
 * La recherche est insensible à la casse.
 * Retourne 1 si au moins une des sections est présente, 0 sinon.
 */
int check_citation_present(const char *document, const char *parameter);

#endif /* CITATION_CHECKER_H */
