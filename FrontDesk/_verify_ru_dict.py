# -*- coding: utf-8 -*-
import json
import pathlib
import re
import sys

sys.stdout.reconfigure(encoding="utf-8")
ROOT = pathlib.Path(r"c:\Development\projects\cafe5\FrontDesk")
d = json.loads((ROOT / "ru_dict.json").read_text(encoding="utf-8"))
src = (ROOT / "_all_sources.txt").read_text(encoding="utf-8").splitlines()
print("keys", len(d))
print("nonempty lines", len(src))
print("unique lines", len(set(src)))
print("all unique covered", set(src) <= set(d.keys()))
checks = [
    "Store input",
    "Whosale price",
    "Expcense account",
    "Complectation",
    "Print precheck",
    "Cashbox",
    "Goods consuption, draft",
    "Costumer debts report",
    "Reedem accumulated",
    "Self cost",
    "Write-off",
    "Partner",
    "Scancode",
    "Draft",
    "Վերահաշվարկել",
    "Մաքսիմալ քանակ",
    "Store movement",
    "Inventory",
    "Debt",
    "Recipe",
    "Unit",
    "Կանխիկ",
    "Անկանխիկ",
    "Լավ",
    "Մաքրել",
    "Ավելացնել",
    "Հրաժարվել",
    "OK",
    "Cancel",
    "Save",
    "%1 — import failed",
    "Date: %1",
    "f_id",
    "form_debts",
    "yyyy-MM-dd",
    "uuid",
    "<html>msg</html>",
    "Whosale",
    "Retail",
]
for s in checks:
    print(repr(s), "->", repr(d.get(s, "MISSING")))
bad = []
for k, v in d.items():
    pk = re.findall(r"%[0-9n]", k)
    pv = re.findall(r"%[0-9n]", v)
    if pk and sorted(pk) != sorted(pv):
        bad.append((k, v, pk, pv))
print("placeholder mismatches", len(bad))
for b in bad[:10]:
    print(" BAD", b)
same = []
for k, v in d.items():
    if k != v:
        continue
    if not re.search(r"[A-Za-z]{4,}", k):
        continue
    if k.startswith(("f_", "form_", "empty=", "o_header.", "o_goods.")):
        continue
    if "http" in k.lower() or "yyyy" in k or "picasso" in k.lower():
        continue
    same.append(k)
print("kept-as-identical candidates", len(same))
for s in same:
    print(" SAME", s)
