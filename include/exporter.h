#ifndef EXPORTER_H
#define EXPORTER_H

#include "editor_core.h"

/**
 * Exporte le contenu de l'éditeur au format RTF (Rich Text Format).
 * Conserve les couleurs et styles appliqués par le formatter.
 */
bool exporter_to_rtf(const EditorContext* ctx, const char* filepath);

/**
 * Exporte le contenu de l'éditeur au format texte brut (UTF-8).
 */
bool exporter_to_text(const EditorContext* ctx, const char* filepath);

#endif // EXPORTER_H
