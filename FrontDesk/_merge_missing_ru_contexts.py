# -*- coding: utf-8 -*-
"""Merge contexts present in FrontDesk.ts but missing from FrontDesk_ru.ts,
filling translations from ru_dict.json."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
HY = ROOT / "FrontDesk.ts"
RU = ROOT / "FrontDesk_ru.ts"
DICT = ROOT / "ru_dict.json"

ctx_re = re.compile(r"<context>\s*<name>(.*?)</name>(.*?)</context>", re.S)
msg_re = re.compile(
    r"(<message>.*?<source>(.*?)</source>\s*)(<translation[^>]*>.*?</translation>)",
    re.S,
)


def unescape_xml(s: str) -> str:
    return (
        s.replace("&amp;", "&")
        .replace("&lt;", "<")
        .replace("&gt;", ">")
        .replace("&quot;", '"')
        .replace("&apos;", "'")
    )


def escape_xml(s: str) -> str:
    return (
        s.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


def translate_context(xml: str, d: dict[str, str]) -> tuple[str, int, int]:
    finished = unfinished = 0
    parts: list[str] = []
    pos = 0
    for m in msg_re.finditer(xml):
        parts.append(xml[pos : m.start()])
        src = unescape_xml(m.group(2))
        tr = d.get(src)
        if tr is None:
            parts.append(m.group(1) + '<translation type="unfinished"></translation>')
            unfinished += 1
        else:
            parts.append(m.group(1) + f"<translation>{escape_xml(tr)}</translation>")
            finished += 1
        pos = m.end()
    parts.append(xml[pos:])
    return "".join(parts), finished, unfinished


def main() -> None:
    hy = HY.read_text(encoding="utf-8")
    ru = RU.read_text(encoding="utf-8")
    d = json.loads(DICT.read_text(encoding="utf-8"))

    hy_ctx = {m.group(1): m.group(0) for m in ctx_re.finditer(hy)}
    ru_names = {m.group(1) for m in ctx_re.finditer(ru)}
    missing = [n for n in hy_ctx if n not in ru_names]
    print(f"missing contexts: {len(missing)}")
    print(f"CR5 missing: {sum(1 for n in missing if n.lower().startswith('cr5'))}")

    insert: list[str] = []
    fin = unf = 0
    for name in missing:
        ctx, f, u = translate_context(hy_ctx[name], d)
        insert.append(ctx)
        fin += f
        unf += u

    print(f"inserted messages: finished={fin} unfinished={unf}")
    if "CR5Goods" in missing:
        print("CR5Goods: restored")

    if not insert:
        print("nothing to do")
        return

    if "</TS>" not in ru:
        raise SystemExit("FrontDesk_ru.ts: missing </TS>")
    new_ru = ru.replace("</TS>", "\n".join(insert) + "\n</TS>", 1)
    RU.write_text(new_ru, encoding="utf-8")
    print(f"wrote {RU} ({len(new_ru)} bytes)")


if __name__ == "__main__":
    main()
