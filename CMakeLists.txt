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

# --- Dossiers d'inclusion ---
include_directories(include)
include_directories(src)
include_directories(src/rules)
include_directories(src/rules/checkers)
include_directories(src/utils)


# 🔹 ===== MODULE RULES (Bibliothèque) =====
set(RULES_SRC
        src/rules/rule_engine.c
        src/rules/rule_parser.c
        src/rules/rule_report.c
        src/rules/checkers/count_checker.c
        src/rules/checkers/regex_checker.c
        src/rules/checkers/section_checker.c
)

add_library(rules_lib STATIC ${RULES_SRC})

# 🔹 ===== APPLICATION PRINCIPALE =====
set(SOURCES
        src/main.c





        # Utilitaires
        src/utils/encoding.c
        src/utils/debug_memory.c
        src/utils/ini_parser.c
        src/utils/memory_pool.c
)

# Création de l'exécutable
add_executable(IntelliEditor ${SOURCES})

# Liens avec les bibliothèques
if(WIN32)
    # On lie l'exécutable à notre bibliothèque de règles et aux libs système Windows
    target_link_libraries(IntelliEditor
            rules_lib
            user32
            gdi32
            comctl32
            comdlg32
            shell32
    )
else()
    target_link_libraries(IntelliEditor rules_lib)
endif()

# --- Gestion des Données ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# --- Tests (Optionnel) ---
option(BUILD_TESTS "Activer les tests unitaires" OFF)
if(BUILD_TESTS)
    enable_testing()
    # add_subdirectory(tests)
endif()