@echo off
echo Building IntelliEditor...

gcc -o IntelliEditor.exe ^
    ui/main_window.c ^
    ui/scintilla_wrapper.c ^
    ui/toolbar.c ^
    ui/statusbar.c ^
    ui/rules_panel.c ^
    ui/dialogs.c ^
    -I. -Iui ^
    -luser32 -lcomctl32 -lgdi32 -lcomdlg32 ^
    -mwindows

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
) else (
    echo Build failed!
)
