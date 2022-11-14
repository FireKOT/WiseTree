@call parameters.bat main.cpp tree.cpp strsort.cpp general.cpp -o main.exe

@if not errorlevel 1 (
    main.exe %*
)