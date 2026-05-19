#ifndef CONFIG_H
#define CONFIG_H

#include "editor_core.h"
#include "ini_parser.h"

/**
 * Loads editor configuration from an INI file.
 * Supported keys (in section [Editor]):
 *   initial_buffer_size  - size_t
 *   tab_width            - size_t
 *   use_utf8              - bool (true/false)
 *
 * The function updates the given EditorContext accordingly.
 * Returns true on success, false on parse error.
 */
bool config_load(const char *filepath, EditorContext *ctx);

/**
 * Saves current editor configuration to an INI file.
 */
bool config_save(const char *filepath, const EditorContext *ctx);

#endif // CONFIG_H
