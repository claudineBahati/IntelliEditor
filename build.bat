@echo off
echo Building IntelliEditor...

gcc -o IntelliEditor.exe ^
    ui/main_window.c ^
    ui/scintilla_wrapper.c ^
    ui/toolbar.c ^
    ui/statusbar.c ^
    ui/rules_panel.c ^
    ui/dialogs.c ^
    src/rules/rule_parser.c ^
    src/rules/rule_engine.c ^
    src/rules/checkers/section_checker.c ^
    src/rules/checkers/contains_checker.c ^
    src/rules/checkers/regex_checker.c ^
    src/rules/checkers/count_checker.c ^
    src/libs/cjson/cJSON.c ^
    src/utils/encoding.c ^
    src/utils/debug_memory.c ^
    src/utils/memory_pool.c ^
    src/editor/editor_core.c ^
    src/editor/history.c ^
    src/editor/formatter.c ^
    src/editor/tokenizer.c ^
    src/editor/exporter.c ^
    src/editor/gap_buffer.c ^
    src/editor/file_io.c ^
    src/editor/search.c ^
    llm/llm_thread.c ^
    -I. -Iui -Isrc -Iinclude -Isrc/rules -Isrc/models -Isrc/libs -Isrc/libs/cjson -Inlp -Illm -Isrc/editor -Isrc/utils -IC:/msys64/ucrt64/include ^
    -L"C:/msys64/ucrt64/lib" ^
    -luser32 -lcomctl32 -lgdi32 -lcomdlg32 -lshell32 -lpcre2-8 ^
    -mwindows

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
) else (
    echo Build failed!
)
