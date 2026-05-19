cmake_minimum_required(VERSION 3.10)
project(IntelliEditor VERSION 1.0 LANGUAGES C)

# --- Configuration du Standard C ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Options de compilation pour Windows ---
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    # Masque la console en arrière-plan pour une application GUI pure
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Dossiers d'inclusion
include_directories(include)
include_directories(src)
include_directories(ui)
include_directories(src/editor)
include_directories(llm)
include_directories(nlp)
include_directories(src/rules)
include_directories(src/rules/checkers)

# ── Sources du moteur de règles (DEV-D) ───────────────────────────
set(RULES_SOURCES
    src/rules/rule_engine.c
    src/rules/rule_parser.c
    src/rules/rule_report.c
    src/rules/checkers/section_checker.c
    src/rules/checkers/count_checker.c
    src/rules/checkers/regex_checker.c
    src/rules/checkers/heading_checker.c
    src/rules/checkers/citation_checker.c
)

add_library(rules_lib STATIC ${RULES_SOURCES})
target_link_libraries(rules_lib pcre2-8)

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
    # Ajout des fichiers LLM (DEV-C)
    llm/llm_thread.c
    # Ajout des fichiers NLP (DEV-A)
    src/nlp/spellcheck.c
)

# Création de l'exécutable
add_executable(IntelliEditor ${SOURCES})

# Liens avec les bibliothèques
if(WIN32)
    target_link_libraries(IntelliEditor
            rules_lib
            user32
            gdi32
            comctl32
            comdlg32
            shell32
    )
endif()

# On lie les bibliothèques (Threads) pour le LLM
find_package(Threads REQUIRED)
target_link_libraries(IntelliEditor PRIVATE Threads::Threads)

# Copier data
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_custom_command(TARGET IntelliEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/data
    )
endif()
