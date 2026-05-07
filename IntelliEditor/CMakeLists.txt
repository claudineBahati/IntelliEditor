cmake_minimum_required(VERSION 3.10)
project(IntelliEditor)

# 1. On demande à CMake de trouver Hunspell
find_package(PkgConfig REQUIRED)
pkg_check_modules(HUNSPELL REQUIRED hunspell)

# 2. On ajoute nos fichiers sources (le code)
add_executable(IntelliEditor
    main.c
    llm/llm_thread.c
    llm/json_parser/json_parser.c     # le  parser
    llm/json_parser/cJSON.c  # La bibliothèque cJSON
    nlp/hunspell_wrap.c
    nlp/tokenizer.c
    nlp/nlp_engine.c
)

# 3. On indique où sont les fichiers .h 
target_include_directories(IntelliEditor PRIVATE 
    . 
    llm 
    llm/json_parser
    nlp
    
    ${HUNSPELL_INCLUDE_DIRS}
)

# 4. On lie la bibliothèque Hunspell au programme final
target_link_libraries(IntelliEditor PRIVATE 
    ${HUNSPELL_LIBRARIES}
)