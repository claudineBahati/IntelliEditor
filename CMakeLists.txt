<<<<<<< HEAD
cmake_minimum_required(VERSION 3.16)
project(intelli C)
=======
cmake_minimum_required(VERSION 3.10)
project(IntelliEditor VERSION 1.0 LANGUAGES C)
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

# --- Configuration du Standard C ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

<<<<<<< HEAD
# ── Options de compilation ─────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()
=======
# --- Options de compilation pour Windows ---
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    # Masque la console en arrière-plan pour une application GUI pure
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# Dossiers d'inclusion
include_directories(include)
include_directories(src)
include_directories(ui)
include_directories(src/editor)
include_directories(llm)
include_directories(nlp)
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f

# ── Sources du moteur de règles (DEV-D) ───────────────────────────
set(RULES_SOURCES
    src/rules/rule_engine.c
<<<<<<< HEAD
=======
    src/rules/checkers/section_checker.c
    src/rules/checkers/contains_checker.c
    src/rules/checkers/count_checker.c
    src/rules/checkers/regex_checker.c
)

add_library(rules_lib STATIC ${RULES_SRC})
target_link_libraries(rules_lib cjson pcre2-8)

# 🔹 ===== MODULE NLP & LLM =====
set(NLP_SRC
    llm/llm_thread.c
)

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

# 🔹 ===== TESTS =====
add_executable(test_rules tests/test_rules.c)
target_link_libraries(test_rules rules_lib cmocka cjson)

add_executable(test_exist_section
    tests/test_exist_section.c
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
    src/rules/rule_parser.c
    src/rules/rule_report.c
    src/rules/checkers/section_checker.c
    src/rules/checkers/count_checker.c
    src/rules/checkers/regex_checker.c
    src/rules/checkers/heading_checker.c
    src/rules/checkers/citation_checker.c
)

# ── Exécutable principal ───────────────────────────────────────────
add_executable(intelli main.c ${RULES_SOURCES})

target_include_directories(intelli PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/rules
    ${CMAKE_SOURCE_DIR}/src/rules/checkers
)

# ── Copie des ressources dans le répertoire de build ──────────────
file(COPY ${CMAKE_SOURCE_DIR}/data
     DESTINATION ${CMAKE_BINARY_DIR})

# ── Tests unitaires (cmocka) ──────────────────────────────────────
# Pour activer : cmake -DBUILD_TESTS=ON ..
option(BUILD_TESTS "Compile les tests unitaires (requiert cmocka)" OFF)

if(BUILD_TESTS)
    enable_testing()

    find_library(CMOCKA_LIB cmocka REQUIRED)
    find_path(CMOCKA_INCLUDE cmocka.h REQUIRED)

    add_executable(test_rules
        tests/test_rules.c
        ${RULES_SOURCES}
    )

    target_include_directories(test_rules PRIVATE
        ${CMAKE_SOURCE_DIR}/src/rules
        ${CMAKE_SOURCE_DIR}/src/rules/checkers
        ${CMOCKA_INCLUDE}
    )

<<<<<<< HEAD
    target_link_libraries(test_rules PRIVATE ${CMOCKA_LIB})

    add_test(NAME RulesTests COMMAND test_rules)
endif()

# ── Message récapitulatif ─────────────────────────────────────────
message(STATUS "")
message(STATUS "=== IntelliEditor — DEV-D (Moteur de Règles) ===")
message(STATUS "  Compilateur  : ${CMAKE_C_COMPILER_ID}")
message(STATUS "  Standard C   : ${CMAKE_C_STANDARD}")
message(STATUS "  Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Tests        : ${BUILD_TESTS}")
message(STATUS "")
=======
# Copier data
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
    add_custom_command(TARGET IntelliEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/data
    )
endif()

# 🔹 Activer les tests
enable_testing()
add_test(NAME RulesTest COMMAND test_rules)
add_test(NAME SectionTest COMMAND test_exist_section)
add_test(NAME RegexCheckerTest COMMAND test_regex_checker)
add_test(NAME CountCheckerTest COMMAND test_count_checker)
>>>>>>> 2b4f0c2296b45bbc0e0a7765efb831705e41095f
