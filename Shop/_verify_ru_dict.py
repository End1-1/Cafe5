# -*- coding: utf-8 -*-
import json
import sys
from pathlib import Path

sys.stdout.reconfigure(encoding="utf-8")
root = Path(r"c:\Development\projects\cafe5\Shop")
d = json.loads((root / "ru_dict.json").read_text(encoding="utf-8"))
sources = (root / "_all_sources.txt").read_text(encoding="utf-8").splitlines()
uniq = list(dict.fromkeys(sources))

samples = [
    "Cash Overage",
    "Cash Shortage",
    "Cash Overage Act",
    "Cash Shortage Act",
    "Session",
    "Receipt",
    "New retail",
    "New whosale",
    "Whosale",
    "Store",
    "Goods",
    "Enter your username and pin",
    "Print precheck",
    "Scancode",
    "Barcode",
    "Draft",
    "Qty",
    "Partner",
    "Fiscal",
    "OK",
    "Cancel",
    "Costumer display",
    "Gift card",
    "Open cashbox session",
    "Cash in drawer",
    "Empty barcode",
    "Shift total",
    "Expected cash",
    "Counted cash",
    "Searchi in",
    "Parnert ID",
    "Avaiable cash",
    "Thank you for visit!",
    "Make draft",
    "Items return",
    "Login",
    "Settings",
    "Shop",
    "Clear",
    "Yes",
    "No",
    "Գումարի չափ",
    "Պահպանել",
    "Ուժի մեջ է մինջև",
]
for k in samples:
    print(f"{k!r} -> {d.get(k, 'MISSING')!r}")

raw = (root / "ru_dict.json").read_text(encoding="utf-8")
print("has Cyrillic", "Склад" in raw)
print("keys", len(d))
print("unique sources", len(uniq))
print("all covered", all(s in d for s in uniq))
print("match", len(d) == len(uniq))
# ensure no ensure_ascii escapes for cyrillic
print("ascii_escape_cyr", "\\u041" in raw)
