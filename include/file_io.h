#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include "gap_buffer.h"

/**
 * Charge le contenu d'un fichier dans le Gap Buffer.
 * Attention : Cela efface le contenu actuel du buffer.
 *
 * @param filepath Le chemin du fichier à ouvrir.
 * @param gb Le pointeur vers le Gap Buffer.
 * @return true si succès, false en cas d'erreur (fichier introuvable, etc).
 */
bool file_load(const char* filepath, GapBuffer* gb);

/**
 * Sauvegarde le contenu complet du Gap Buffer dans un fichier sur le disque.
 *
 * @param filepath Le chemin du fichier de destination.
 * @param gb Le pointeur vers le Gap Buffer.
 * @return true si succès, false en cas d'erreur.
 */
bool file_save(const char* filepath, const GapBuffer* gb);

#endif // FILE_IO_H
