set HOST=sh
set PATH=%PATH%;C:\Program Files\WinRAR;C:\Program Files\MariaDB 10.7\bin
set DATE=%date:.=%_%time::=%
echo %DATE%
set BACKUP_DIR=d:\backup
set TEMP_DIR=d:\temp
mkdir %BACKUP_DIR%\%HOST%
mkdir %TEMP_DIR%
mariadb-dump.exe -B -h127.0.0.1 cafe5 -uroot -proot5 > %TEMP_DIR%\%DATE% 
rar a -m5 -df -ep %BACKUP_DIR%\%HOST%\%DATE%.rar %TEMP_DIR%\%DATE% 
del /Q d:\temp\*