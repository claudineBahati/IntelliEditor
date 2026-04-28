cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

# Force le mode console pour éviter l'erreur WinMain
set(CMAKE_EXE_LINKER_FLAGS "-mconsole")

# 1. On indique où trouver les headers (.h)
include_directories(llm)
include_directories(llama.cpp/include)
include_directories(llama.cpp/common)

# 2. On ajoute tes fichiers sources
set(SOURCES 
    main.c
    llm/llm_thread.c
)

# 3. On crée l'exécutable final
add_executable(IntelliEditor ${SOURCES})

# 4. On lie les bibliothèques (Threads)
find_package(Threads REQUIRED)
target_link_libraries(IntelliEditor PRIVATE Threads::Threads)