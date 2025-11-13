@echo off
rem --- Активуємо MSVC середовище x64 ---
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

rem --- Перехід у папку проекту ---
cd /d E:\QtProjects\HwInfoTest

rem --- Створення build папки ---
if not exist build mkdir build
cd build

rem --- Генерація Makefiles через CMake з Qt6 ---
cmake .. -G "NMake Makefiles" -DQt6_DIR="C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6"

rem --- Збірка проекту ---
nmake

rem --- Запуск exe, якщо збірка успішна ---
if exist hwinfo_test.exe hwinfo_test.exe

pause
