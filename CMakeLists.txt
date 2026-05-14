cmake_minimum_required(VERSION 3.10)
project(IntelliEditor VERSION 1.0 LANGUAGES C)

# Utilisation du standard C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options de compilation pour Windows (MinGW)
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    # Pour déboguer, gardez la console activée
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# Dossiers d'inclusion
include_directories(include)
include_directories(src)
include_directories(ui)
include_directories(src/editor)

# 🔹 ===== MODULE RULES =====
set(RULES_SRC
    src/rules/rule_parser.c
)

add_library(rules_lib ${RULES_SRC})
# target_link_libraries(rules_lib cjson) # Commented out if cjson is not present yet

# Liste des fichiers source de l'application principale
set(SOURCES
    src/main.c
    src/utils/encoding.c
    src/utils/debug_memory.c
    src/editor/editor_core.c
    src/editor/history.c
    src/editor/formatter.c
    src/editor/tokenizer.c
    src/editor/exporter.c
    src/editor/gap_buffer.c
    src/utils/ini_parser.c
    src/editor/file_io.c
    src/editor/search.c
    # Ajout des fichiers UI (DEV-B)
    ui/main_window.c
    ui/scintilla_wrapper.c
    ui/toolbar.c
    ui/statusbar.c
    ui/rules_panel.c
    ui/dialogs.c
    src/utils/memory_pool.c
)

# Création de l'exécutable
add_executable(IntelliEditor ${SOURCES})

# Ajout des tests unitaires de base pour la fondation technique
option(BUILD_TESTS "Build unit tests" ON)
enable_testing()
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Bibliothèques Windows standard nécessaires pour l'interface
if(WIN32)
    target_link_libraries(IntelliEditor user32 gdi32 comctl32 comdlg32 shell32 rules_lib)
endif()

# 🔹 ===== TESTS =====
# find_package(cmocka REQUIRED) # Commented out to avoid build break if not installed

# add_executable(test_rules
#     tests/test_rules.c
# )

# target_link_libraries(test_rules
#     rules_lib
#     cmocka
#     cjson
# )

# 🔹 Activer les tests
# enable_testing()
# add_test(NAME RulesTest COMMAND test_rules)
