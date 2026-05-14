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

# Dossiers d'inclusion
include_directories(include)
include_directories(src)
include_directories(ui)
include_directories(src/editor)
include_directories(llm)
include_directories(nlp)

# 🔹 ===== MODULE RULES =====
set(RULES_SRC
    src/rules/rule_parser.c
    src/rules/rule_engine.c
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
    src/rules/rule_parser.c
    src/rules/rule_engine.c
    src/rules/checkers/section_checker.c
    src/rules/checkers/contains_checker.c
    src/rules/checkers/count_checker.c
    src/rules/checkers/regex_checker.c
)
target_link_libraries(test_exist_section rules_lib cmocka cjson pcre2-8)

add_executable(test_regex_checker
    tests/test_regex_checker.c
    src/rules/checkers/regex_checker.c
)
target_link_libraries(test_regex_checker rules_lib cmocka pcre2-8)

add_executable(test_count_checker
    tests/test_count_checker.c
    src/rules/checkers/count_checker.c
)
target_link_libraries(test_count_checker
    cmocka
)

add_executable(test_section_order_checker
    tests/test_section_order_checker.c
    src/rules/checkers/section_checker.c
)

target_link_libraries(test_section_order_checker
    cmocka
    cjson
)

# 🔹 Copier les DLL runtime (Windows)
set(MSYS2_BIN_DIR "C:/msys64/mingw64/bin")

# DLL pour cjson
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libcjson-1.dll
        ${CMAKE_BINARY_DIR}
)

# DLL pour cmocka
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libcmocka.dll
        ${CMAKE_BINARY_DIR}
)

# DLL runtime GCC/threads
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libgcc_s_seh-1.dll
        ${CMAKE_BINARY_DIR}
)
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libwinpthread-1.dll
        ${CMAKE_BINARY_DIR}
)

# Copier la DLL PCRE2 
add_custom_command(TARGET test_exist_section POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libpcre2-8-0.dll
        ${CMAKE_BINARY_DIR}
)
add_custom_command(TARGET test_regex_checker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MSYS2_BIN_DIR}/libpcre2-8-0.dll
        ${CMAKE_BINARY_DIR}
)

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
