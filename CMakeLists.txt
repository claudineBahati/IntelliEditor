cmake_minimum_required(VERSION 3.10)
project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)

# Exécutable test_loader
add_executable(test_loader
    tests/test_loader.c
    rules/rule_parser.c
    rules/rule_engine.c
)

target_include_directories(test_loader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rules
)

target_link_libraries(test_loader PRIVATE cjson pcre2-8 cmocka)


add_executable(test_parser
    tests/test_parser.c
    rules/rule_parser.c
    rules/rule_engine.c
)

target_include_directories(test_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rules
)

target_link_libraries(test_parser PRIVATE cjson pcre2-8 cmocka)


enable_testing()
add_test(NAME test_loader COMMAND test_loader)
add_test(NAME test_parser COMMAND test_parser)
