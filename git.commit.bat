c:\Projects\Cafe5\mariadb-dump.exe -uroot -posyolia elina > c:\Projects\Cafe5\db.cafe5.sql
c:\Projects\Cafe5\mariadb-dump.exe -uroot -posyolia server5 > c:\Projects\Cafe5\db.server5.sql
del c:\projects\cafe5\db.cafe5.rar
rar a -df -ma5 -m5 -ep c:\Projects\Cafe5\db.cafe5.rar c:\Projects\Cafe5\db.cafe5.sql c:\Projects\Cafe5\db.server5.sql
git add .
git commit
git push

pause