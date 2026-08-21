# -*- coding: utf-8 -*-
"""Rebuild Shop_ru.ts from Shop.ts applying ru_dict.json."""
from __future__ import annotations

import json
import re
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SRC_TS = ROOT / "Shop.ts"
OUT_TS = ROOT / "Shop_ru.ts"
DICT_PATH = ROOT / "ru_dict.json"

# Extra strings for attendance / staff / printer / sales-by-assistant
EXTRA: dict[str, str] = {
    "Attendance": "Посещаемость",
    "Enter login and password": "Введите логин и пароль",
    "Confirm checkout for %1": "Подтвердите выход для %1",
    "Confirm checkin for %1": "Подтвердите вход для %1",
    "Checkout completed": "Выход выполнен",
    "Checkin completed": "Вход выполнен",
    "No staff checked in": "Нет сотрудников на смене",
    "Printer: not configured": "Принтер не настроен",
    "Printer: local": "Принтер: локальный",
    "Printer: local (%1)": "Принтер: локальный (%1)",
    "Printer: %1 @ %2": "Принтер: %1 @ %2",
    "Printer: %1": "Принтер: %1",
    "Login": "Логин",
    "Password": "Пароль",
    "Staff": "Персонал",
    "By Sales Assistant": "По продавцу",
    "Sales assistant": "Продавец",
    "Orders": "Заказы",
    "Bank": "Перевод",
}


def main() -> None:
    d = json.loads(DICT_PATH.read_text(encoding="utf-8"))
    d.update(EXTRA)
    DICT_PATH.write_text(
        json.dumps(d, ensure_ascii=False, indent=0, sort_keys=True) + "\n",
        encoding="utf-8",
    )

    text = SRC_TS.read_text(encoding="utf-8")
    text = text.replace('language="hy_AM"', 'language="ru_RU"', 1)

    # Parse with ElementTree — keep structure simple via regex on messages
    def replace_translation(match: re.Match[str]) -> str:
        block = match.group(0)
        src_m = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        if not src_m:
            return block
        # unescape basic XML entities in source
        source = (
            src_m.group(1)
            .replace("&lt;", "<")
            .replace("&gt;", ">")
            .replace("&amp;", "&")
            .replace("&quot;", '"')
            .replace("&apos;", "'")
        )
        tr = d.get(source)
        if tr is None:
            # keep unfinished empty for missing
            return re.sub(
                r"<translation[^>]*>.*?</translation>",
                '<translation type="unfinished"></translation>',
                block,
                count=1,
                flags=re.DOTALL,
            )
        esc = (
            tr.replace("&", "&amp;")
            .replace("<", "&lt;")
            .replace(">", "&gt;")
            .replace('"', "&quot;")
        )
        return re.sub(
            r"<translation[^>]*>.*?</translation>",
            f"<translation>{esc}</translation>",
            block,
            count=1,
            flags=re.DOTALL,
        )

    out = re.sub(r"<message>.*?</message>", replace_translation, text, flags=re.DOTALL)
    OUT_TS.write_text(out, encoding="utf-8")

    unfinished = len(re.findall(r'type="unfinished"', out))
    translated = len(re.findall(r"<translation>[^<]", out))
    print(f"Wrote {OUT_TS}")
    print(f"translated~{translated}, unfinished={unfinished}, dict={len(d)}")


if __name__ == "__main__":
    main()
