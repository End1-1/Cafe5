# -*- coding: utf-8 -*-
"""Add cooking/service-print translations and refresh .ts/.qm for Shop, FrontDesk, CookingProgress."""
from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
QTBIN = Path(r"C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin")
LUPDATE = QTBIN / "lupdate.exe"
LRELEASE = QTBIN / "lrelease.exe"

RU: dict[str, str] = {
    "Cooking progress": "Очередь готовки",
    "Preparing": "Готовятся",
    "Ready": "Готово",
    "Served": "Отдано",
    "Accepted": "Принято",
    "Cooking": "Готовится",
    "Select lines": "Выберите строки",
    "Mark as served to the guest?": "Отметить как отдано клиенту?",
    "Service print": "Сервис-печать",
    "New order": "Новый заказ",
    "Order no": "№ заказа",
    "Printer not found": "Принтер не найден",
    "Printer: ": "Принтер: ",
    "Exit": "Выход",
    "Refresh": "Обновить",
    "Order": "Заказ",
    "Dish": "Товар",
    "Qty": "Кол-во",
    "Status": "Статус",
    "Close": "Закрыть",
    "Date": "Дата",
    "Time": "Время",
    "Staff": "Сотрудник",
    "Table": "Стол",
    "Print error": "Ошибка печати",
    "An instance of application already running": "Экземпляр приложения уже запущен",
    "Program account could not log in. Would you like to open settings?": "Не удалось войти под учётной записью программы. Открыть настройки?",
    "Yes": "Да",
    "No": "Нет",
}

HY: dict[str, str] = {
    "Cooking progress": "Պատրաստման հերթ",
    "Preparing": "Պատրաստվում են",
    "Ready": "Պատրաստ է",
    "Served": "Հանձնված է",
    "Accepted": "Ընդունված",
    "Cooking": "Պատրաստվում է",
    "Select lines": "Ընտրեք տողերը",
    "Mark as served to the guest?": "Հանձնե՞լ հաճախորդին",
    "Service print": "Սերվիս տպիչ",
    "New order": "Նոր պատվեր",
    "Order no": "Պատվերի համար",
    "Printer not found": "Տպիչը չի գտնվել",
    "Printer: ": "Տպիչ՝ ",
    "Exit": "Ելք",
    "Refresh": "Թարմացնել",
    "Order": "Պատվեր",
    "Dish": "Ապրանք",
    "Qty": "Քանակ",
    "Status": "Կարգավիճակ",
    "Close": "Փակել",
    "Date": "Ամսաթիվ",
    "Time": "Ժամ",
    "Staff": "Աշխատակից",
    "Table": "Սեղան",
    "Print error": "Տպման սխալ",
    "An instance of application already running": "Ծրագրի օրինակն արդեն աշխատում է",
    "Program account could not log in. Would you like to open settings?": "Չհաջողվեց մուտք գործել ծրագրի հաշվով։ Բացե՞լ կարգավորումները",
    "Yes": "Այո",
    "No": "Ոչ",
}


def merge_json(path: Path, extra: dict[str, str]) -> dict[str, str]:
    data: dict[str, str] = {}
    if path.exists():
        data = json.loads(path.read_text(encoding="utf-8"))
    data.update(extra)
    path.write_text(json.dumps(data, ensure_ascii=False, indent=0, sort_keys=True) + "\n", encoding="utf-8")
    return data


def xml_unescape(s: str) -> str:
    return (
        s.replace("&lt;", "<")
        .replace("&gt;", ">")
        .replace("&amp;", "&")
        .replace("&quot;", '"')
        .replace("&apos;", "'")
    )


def xml_escape(s: str) -> str:
    return (
        s.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def apply_existing_ts_as_dict(ts_path: Path) -> dict[str, str]:
    out: dict[str, str] = {}
    if not ts_path.exists():
        return out
    text = ts_path.read_text(encoding="utf-8")
    for m in re.finditer(r"<message>.*?</message>", text, flags=re.DOTALL):
        block = m.group(0)
        if 'type="unfinished"' in block:
            continue
        src_m = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        tr_m = re.search(r"<translation>(.*?)</translation>", block, re.DOTALL)
        if not src_m or not tr_m or not tr_m.group(1).strip():
            continue
        out[xml_unescape(src_m.group(1))] = xml_unescape(tr_m.group(1))
    return out


def fill_ts(ts_path: Path, dictionary: dict[str, str], language: str | None = None) -> None:
    text = ts_path.read_text(encoding="utf-8")
    if language:
        text = re.sub(r'language="[^"]+"', f'language="{language}"', text, count=1)

    def repl(match: re.Match[str]) -> str:
        block = match.group(0)
        src_m = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        if not src_m:
            return block
        source = xml_unescape(src_m.group(1))
        tr = dictionary.get(source)
        if tr is None:
            return block
        return re.sub(
            r"<translation[^>]*>.*?</translation>",
            f"<translation>{xml_escape(tr)}</translation>",
            block,
            count=1,
            flags=re.DOTALL,
        )

    out = re.sub(r"<message>.*?</message>", repl, text, flags=re.DOTALL)
    ts_path.write_text(out, encoding="utf-8")
    unfinished = len(re.findall(r'type="unfinished"', out))
    print(f"  filled {ts_path.name}: unfinished={unfinished}")


def run(cmd: list[str], cwd: Path | None = None) -> None:
    print(">", " ".join(cmd))
    env = dict(os.environ)
    env["PATH"] = str(QTBIN) + ";" + env.get("PATH", "")
    r = subprocess.run(cmd, cwd=str(cwd) if cwd else None, env=env)
    if r.returncode != 0:
        raise SystemExit(f"command failed: {cmd} -> {r.returncode}")


def build_ru_ts_from_hy(src_ts: Path, out_ts: Path, dictionary: dict[str, str]) -> None:
    text = src_ts.read_text(encoding="utf-8").replace('language="hy_AM"', 'language="ru_RU"', 1)

    def repl(match: re.Match[str]) -> str:
        block = match.group(0)
        src_m = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        if not src_m:
            return block
        source = xml_unescape(src_m.group(1))
        tr = dictionary.get(source)
        if tr is None:
            return re.sub(
                r"<translation[^>]*>.*?</translation>",
                '<translation type="unfinished"></translation>',
                block,
                count=1,
                flags=re.DOTALL,
            )
        return re.sub(
            r"<translation[^>]*>.*?</translation>",
            f"<translation>{xml_escape(tr)}</translation>",
            block,
            count=1,
            flags=re.DOTALL,
        )

    out = re.sub(r"<message>.*?</message>", repl, text, flags=re.DOTALL)
    out_ts.write_text(out, encoding="utf-8")
    print(f"  wrote {out_ts.name}: unfinished={len(re.findall(r'type=\"unfinished\"', out))}")


def main() -> None:
    shop = ROOT / "Shop"
    fd = ROOT / "FrontDesk"
    cp = ROOT / "CookingProgress"

    shop_ru = merge_json(shop / "ru_dict.json", RU)
    shop_hy = merge_json(shop / "_hy_manual.json", HY)
    fd_ru = merge_json(fd / "ru_dict.json", RU)

    # --- Shop ---
    run(
        [
            str(LUPDATE),
            ".",
            "../Cafe5",
            "../Classes",
            "../Forms",
            "-ts",
            "Shop.ts",
            "-recursive",
            "-no-obsolete",
        ],
        cwd=shop,
    )
    hy_dict = apply_existing_ts_as_dict(shop / "Shop.ts")
    hy_dict.update(shop_hy)
    fill_ts(shop / "Shop.ts", hy_dict, "hy_AM")

    run([sys.executable, str(shop / "_apply_ru_dict_to_ts.py")], cwd=shop)
    fill_ts(shop / "Shop_ru.ts", shop_ru, "ru_RU")

    run([str(LRELEASE), "Shop.ts", "-qm", "Shop.qm"], cwd=shop)
    run([str(LRELEASE), "Shop_ru.ts", "-qm", "Shop_ru.qm"], cwd=shop)

    # --- FrontDesk: update both ts in place (do not wipe FrontDesk_ru) ---
    run(
        [
            str(LUPDATE),
            ".",
            "../Editors",
            "../Cafe5",
            "../Classes",
            "../Forms",
            "-ts",
            "FrontDesk.ts",
            "FrontDesk_ru.ts",
            "-recursive",
            "-no-obsolete",
        ],
        cwd=fd,
    )
    hy_fd = apply_existing_ts_as_dict(fd / "FrontDesk.ts")
    hy_fd.update(HY)
    fill_ts(fd / "FrontDesk.ts", hy_fd)
    # FrontDesk_ru: keep existing + fill known keys from dict
    ru_fd = apply_existing_ts_as_dict(fd / "FrontDesk_ru.ts")
    ru_fd.update(fd_ru)
    fill_ts(fd / "FrontDesk_ru.ts", ru_fd)

    run([str(LRELEASE), "FrontDesk.ts", "-qm", "FrontDesk.qm"], cwd=fd)
    run([str(LRELEASE), "FrontDesk_ru.ts", "-qm", "FrontDesk_ru.qm"], cwd=fd)

    # --- CookingProgress (local UI + splash/connection/message only) ---
    cp_ts = cp / "CookingProgress.ts"
    if cp_ts.exists():
        cp_ts.unlink()
    run(
        [
            str(LUPDATE),
            ".",
            "../Forms/dlgsplashscreen.cpp",
            "../Forms/dlgsplashscreen.ui",
            "../Forms/c5connectiondialog.cpp",
            "../Forms/c5connectiondialog.ui",
            "../Cafe5/c5message.cpp",
            "-ts",
            "CookingProgress.ts",
            "-no-obsolete",
        ],
        cwd=cp,
    )
    hy_cp = apply_existing_ts_as_dict(shop / "Shop.ts")
    hy_cp.update(apply_existing_ts_as_dict(fd / "FrontDesk.ts"))
    hy_cp.update(shop_hy)
    fill_ts(cp_ts, hy_cp, "hy_AM")
    # For any remaining unfinished, use English source as temporary translation
    fill_ts(cp_ts, {}, "hy_AM")  # no-op for missing; handle below
    text_cp = cp_ts.read_text(encoding="utf-8")

    def fill_missing_with_source(match: re.Match[str]) -> str:
        block = match.group(0)
        if 'type="unfinished"' not in block and re.search(r"<translation>[^<]", block):
            return block
        src_m = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        if not src_m:
            return block
        source = xml_unescape(src_m.group(1))
        return re.sub(
            r"<translation[^>]*>.*?</translation>",
            f"<translation>{xml_escape(source)}</translation>",
            block,
            count=1,
            flags=re.DOTALL,
        )

    text_cp = re.sub(r"<message>.*?</message>", fill_missing_with_source, text_cp, flags=re.DOTALL)
    cp_ts.write_text(text_cp, encoding="utf-8")

    cp_ru_dict = dict(shop_ru)
    cp_ru_dict.update(RU)
    build_ru_ts_from_hy(cp_ts, cp / "CookingProgress_ru.ts", cp_ru_dict)
    # fill missing ru with source
    text_ru = (cp / "CookingProgress_ru.ts").read_text(encoding="utf-8")
    text_ru = re.sub(r"<message>.*?</message>", fill_missing_with_source, text_ru, flags=re.DOTALL)
    (cp / "CookingProgress_ru.ts").write_text(text_ru, encoding="utf-8")

    run([str(LRELEASE), "CookingProgress.ts", "-qm", "CookingProgress.qm"], cwd=cp)
    run([str(LRELEASE), "CookingProgress_ru.ts", "-qm", "CookingProgress_ru.qm"], cwd=cp)

    print("Done.")


if __name__ == "__main__":
    main()
