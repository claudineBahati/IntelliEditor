#ifndef REGEX_CHECKER_H
#define REGEX_CHECKER_H

/*
 * Vérifie la présence d'un mot ou d'une expression interdite.
 * *pattern* peut être :
 *   - un mot simple        : "plagiat"
 *   - un motif alternatif  : "\bje\b|\bj'|\bme\b"  (séparateur |)
 * *flags* : chaîne optionnelle pouvant contenir "case_insensitive".
 *
 * Retourne 1 si le motif est trouvé (règle violée), 0 sinon.
 */
int check_forbidden_pattern(const char *text,
                             const char *pattern,
                             const char *flags);

/*
 * Vérifie la présence obligatoire d'une expression dans le document.
 * Retourne 1 si trouvée (règle respectée), 0 sinon.
 */
int check_required_pattern(const char *text,
                            const char *pattern,
                            const char *flags);

/* ── Aliases pour compatibilité avec le code existant ────────────── */
int check_forbidden_word(const char *text, const char *word);
int check_required_expression(const char *text, const char *expression);

#endif /* REGEX_CHECKER_H */
