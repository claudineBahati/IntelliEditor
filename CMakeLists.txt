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

# Dossiers d'inclusion (où chercher les fichiers .h)
include_directories(include)
include_directories(src)
include_directories(src/editor)

# Liste des fichiers source (on commence avec main.c)
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
    src/utils/memory_pool.c
    src/nlp/spellcheck.c
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
    target_link_libraries(IntelliEditor user32 gdi32 comctl32)
endif()
