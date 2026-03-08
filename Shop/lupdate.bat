@echo off
:: 1. Переходим в папку проекта
cd /d "C:\Development\projects\cafe5\Shop"

:: 2. Запускаем lupdate с ОБЯЗАТЕЛЬНЫМИ кавычками для пути к программе
:: И ОБЯЗАТЕЛЬНЫМИ кавычками для путей с точками
"D:\soft\Qt\6.8.0\msvc2022_64\bin\lupdate.exe" ^
    "." "../Cafe5" "../Classes" "../Forms" ^
    -ts "Shop.ts" -recursive -no-obsolete

pause