#ifndef HEADING_CHECKER_H
#define HEADING_CHECKER_H

/*
 * Vérifie le format des titres dans le document.
 *
 * *parameter* est une chaîne JSON objet du type :
 *   { "level": 1, "case": "uppercase" }
 *
 * Niveaux de titre reconnus (Markdown-style) :
 *   H1 → ligne commençant par "# "
 *   H2 → ligne commençant par "## "
 *   H3 → ligne commençant par "### "
 *   H4 → ligne commençant par "#### "
 *
 * Formats supportés :
 *   "uppercase"  → tous les caractères alphabétiques en majuscules
 *   "lowercase"  → tous les caractères alphabétiques en minuscules
 *   "capitalize" → première lettre en majuscule
 *
 * Retourne 1 si TOUS les titres du niveau indiqué respectent le format,
 * 0 si au moins un titre ne le respecte pas.
 * Retourne 1 (neutre) si aucun titre du niveau donné n'est trouvé.
 */
int check_heading_format(const char *document, const char *parameter);

#endif /* HEADING_CHECKER_H */
