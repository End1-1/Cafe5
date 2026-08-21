# -*- coding: utf-8 -*-
"""Build FrontDesk/ru_dict.json — professional RU translations for Cafe5 POS/ERP."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCES = (ROOT / "_all_sources.txt").read_text(encoding="utf-8").splitlines()
TRANS_PATH = ROOT / "_ru_trans_raw.json"


def keep_as_is(s: str) -> bool:
    if not s.strip():
        return True
    if re.fullmatch(r"f_[A-Za-z0-9_]+", s):
        return True
    if s.startswith("empty="):
        return True
    if s.startswith("form_"):
        return True
    if s.startswith("o_header.") or s.startswith("o_goods."):
        return True
    if s in {
        "uuid",
        "int",
        "double",
        "kb",
        "x",
        "H",
        "M",
        "S",
        "I",
        "II",
        "III",
        "IV",
        "V",
        "VI",
        "VII",
        "TS",
        "QR",
        "NN",
        "V1",
        "V2",
        "V3",
        "TLD",
        "STAFF",
        "A1",
        "A2",
        "A3",
        "C",
        "MR",
        "M+",
        "IN/OUT",
        "In",
        "X",
        "RecID",
        "Id",
        "ID",
        "Crn",
        "Adgt",
        "Adg",
        "dtname",
        "part_name",
        "doctype",
        "date1",
        "date2",
        "docstate",
        "paid",
        "supplier",
        "operator",
        "payment",
        "dishtype",
        "goodstype",
        "matgroup",
        "inout",
        "storein",
        "storeout",
        "account",
        "wsupplier",
        "material",
        "d1",
        "d2",
        "product",
        "workerstate",
        "workerpos",
        "process",
        "worker",
        "teamlead",
        "partner",
        "date",
        "class",
        "showzero",
        "showdrafts",
        "class1",
        "class2",
        "class3",
        "class4",
        "azction",
        "menu",
        "dept",
        "type",
        "state",
        "code",
        "showqty",
        "showamount",
        "typeofdish",
        "department",
        "stateorder",
        "storeprice",
        "retailprice",
        "usergroup",
        "userstate",
        "pricecurr",
        "currency",
        "reason",
        "hall",
        "table",
        "group",
        "store",
        "dish",
        "dishpart",
        "goods",
        "buyer",
        "staff",
        "shift",
        "orderstate",
        "employee",
        "materialgroup",
        "xlaS",
        "store_calc_queue",
        "TextLabel",
        "CommandLinkButton",
        "Picasso",
        "UUID",
        "EAN13",
        "AS invoice",
        "AS Retail",
        "ArmSoft",
        "Armenian Soft",
        "Idram",
        "TelCell",
        "Breeze service",
        "PicassCloud: Office",
        "Web: www.picasso.am",
        "(C) 2017-2026 Kudryashov Vasili",
        "<html>msg</html>",
        "https://money.idram.am/api/History/Search",
        "https://cashback.idram.am/api/history/searchbyorder",
        "yyyy-MM-dd HH:mm:ss",
        "yyyy-MM-dd",
        "dd.MM.yyyy HH:mm",
        "HH:mm",
        "##:##:##",
        "50mm",
        "80mm",
        "XML files (*.xml)",
        "Excel (*.xlsx)",
        "000111222",
        "+374 99 999-999",
        "+374  -",
        "27.07.2019 22:16",
        "01/01/2024 - 01/01/2025",
        " %",
        "%",
        "...",
        "SalesCounted",
    }:
        return True
    # numbers / operators / symbols only
    if re.fullmatch(r"[\d\s.,:+\-*/÷=%‹›⏎—\\]*", s) and any(ch.isdigit() or ch in "+-*/÷=.%‹›⏎—," for ch in s):
        return True
    if s in {"+", "-", "*", "/", "=", ".", ",", "÷", "+/-", "‹", "›", "—", "⏎"}:
        return True
    if re.fullmatch(r"%[0-9n]", s):
        return True
    if re.fullmatch(r"%[0-9n]:\s*%[0-9n]", s):
        return True
    # technical snake_case ids
    if re.fullmatch(r"[a-z][a-z0-9_]*", s) and "_" in s:
        return True
    return False


def main() -> None:
    raw = json.loads(TRANS_PATH.read_text(encoding="utf-8"))
    out: dict[str, str] = {}
    missing: list[str] = []
    for src in SOURCES:
        if src in out:
            continue
        if keep_as_is(src) and src not in raw:
            out[src] = src
        elif src in raw:
            out[src] = raw[src]
        elif keep_as_is(src):
            out[src] = src
        else:
            missing.append(src)
            out[src] = src
    out_path = ROOT / "ru_dict.json"
    out_path.write_text(json.dumps(out, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"keys={len(out)} sources_lines={len(SOURCES)} unique={len(set(SOURCES))} missing={len(missing)}")
    if missing:
        (ROOT / "_ru_missing.txt").write_text("\n".join(missing), encoding="utf-8")
        print("wrote _ru_missing.txt")


if __name__ == "__main__":
    main()
