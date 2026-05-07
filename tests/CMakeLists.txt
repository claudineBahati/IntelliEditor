set(TEST_SOURCES
    test_core.c
    ../src/utils/encoding.c
    ../src/utils/debug_memory.c
    ../src/editor/gap_buffer.c
    ../src/editor/search.c
    ../src/editor/file_io.c
    ../src/utils/ini_parser.c
    ../src/editor/history.c
    ../src/editor/formatter.c
    ../src/editor/tokenizer.c
    ../src/editor/exporter.c
    ../src/editor/editor_core.c
    ../src/utils/memory_pool.c
)

add_executable(IntelliEditorTests ${TEST_SOURCES})

target_include_directories(IntelliEditorTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME IntelliEditorTests COMMAND IntelliEditorTests)
