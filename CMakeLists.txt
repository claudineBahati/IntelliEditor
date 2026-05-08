cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)

# 🔹 Inclure headers du projet
include_directories(src)

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
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
)

# 🔹 Activer les tests
enable_testing()
add_test(NAME RulesTest COMMAND test_rules)
add_test(NAME SectionTest COMMAND test_exist_section)
add_test(NAME RegexCheckerTest COMMAND test_regex_checker)
add_test(NAME CountCheckerTest COMMAND test_count_checker)