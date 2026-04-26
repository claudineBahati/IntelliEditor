cmake_minimum_required(VERSION 3.10)
project(IntelliEditor VERSION 1.0 LANGUAGES C)

# Utilisation du standard C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options de compilation pour Windows (MinGW)
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    # Pour éviter d'ouvrir une console en arrière-plan lors du lancement de l'application GUI
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

# Dossiers d'inclusion (où chercher les fichiers .h)
include_directories(include)
include_directories(src)

# Liste des fichiers source (on commence avec main.c)
set(SOURCES
    src/main.c
    src/utils/encoding.c
)

# Création de l'exécutable
add_executable(IntelliEditor ${SOURCES})

# Bibliothèques Windows standard nécessaires pour l'interface
if(WIN32)
    target_link_libraries(IntelliEditor user32 gdi32 comctl32)
endif()
