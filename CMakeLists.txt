cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)

# 🔹 Inclure headers du projet
include_directories(src)

# 🔹 Trouver les librairies
find_package(cmocka REQUIRED)

# 🔹 ===== MODULE RULES =====
set(RULES_SRC
    src/rules/rule_parser.c
)

add_library(rules_lib ${RULES_SRC})

target_link_libraries(rules_lib
    cjson
)

# 🔹 ===== TESTS =====
add_executable(test_rules
    tests/test_rules.c
)

target_link_libraries(test_rules
    rules_lib
    cmocka
    cjson
)

# 🔹 Copier les DLL (solution Windows fiable)
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        C:/msys64/mingw64/bin/libcjson-1.dll
        ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        C:/msys64/mingw64/bin/libcmocka.dll
        ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        C:/msys64/mingw64/bin/libgcc_s_seh-1.dll
        ${CMAKE_BINARY_DIR}
)

add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        C:/msys64/mingw64/bin/libwinpthread-1.dll
        ${CMAKE_BINARY_DIR}
)
add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        ${CMAKE_BINARY_DIR}/data
)
# 🔹 Activer les tests
enable_testing()

add_test(NAME RulesTest COMMAND test_rules)