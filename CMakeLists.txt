cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)


# 🔹 Inclure headers
include_directories(src)
include_directories(C:/msys64/mingw64/include)

# 🔹 Trouver les libs
find_package(cmocka REQUIRED)
find_package(cJSON REQUIRED)

# 🔹 Source rules
set(RULES_SRC
    src/rules/rule_parser.c
)

# 🔹 Executable test
add_executable(test_rules
    tests/test_rules.c
    src/rules/rule_parser.c
)

add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        C:/msys64/mingw64/bin/libcjson-1.dll
        ${CMAKE_BINARY_DIR}


)

add_custom_command(TARGET test_rules POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        C:/msys64/mingw64/bin/libcmocka.dll
        ${CMAKE_BINARY_DIR}
)




# 🔹 Link libs
target_link_libraries(test_rules
    cmocka
    cjson
)

# 🔹 Activer tests
enable_testing()

add_test(NAME RulesTest COMMAND test_rules)