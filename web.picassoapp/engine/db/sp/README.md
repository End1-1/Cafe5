# Stored procedures / functions

## Как работать

1. **Смотри и правь** отдельные файлы здесь (`sp/*.sql`).
2. **Собери один скрипт** из корня `engine/db/`:

```bat
build_sp.bat
```

или

```bash
python build_sp_sql.py
```

3. **На сервере выполни только** `../sp.sql` — он пересоздаёт все рутины (`DROP` + `CREATE`).

```bash
mysql -uroot -p YOUR_DB < sp.sql
```

`sp.sql` руками не править — он перезаписывается сборщиком.
