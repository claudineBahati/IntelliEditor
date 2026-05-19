cmake_minimum_required(VERSION 3.16)
project(intelli C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ── Options de compilation ─────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

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
