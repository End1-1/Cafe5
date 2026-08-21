# -*- coding: utf-8 -*-
"""Rebuild CookingProgress.ts/.qm from local UI + splash/connection/message strings."""
import json
import os
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent
QTBIN = Path(r"C:\Development\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin")
cp = ROOT / "CookingProgress"
shop = ROOT / "Shop"
fd = ROOT / "FrontDesk"

env = dict(os.environ)
env["PATH"] = str(QTBIN) + ";" + env.get("PATH", "")


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


def apply_existing(ts_path: Path) -> dict[str, str]:
    out: dict[str, str] = {}
    if not ts_path.exists():
        return out
    text = ts_path.read_text(encoding="utf-8")
    for m in re.finditer(r"<message>.*?</message>", text, flags=re.DOTALL):
        block = m.group(0)
        if 'type="unfinished"' in block:
            continue
        sm = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        tm = re.search(r"<translation>(.*?)</translation>", block, re.DOTALL)
        if sm and tm and tm.group(1).strip():
            out[xml_unescape(sm.group(1))] = xml_unescape(tm.group(1))
    return out


def fill_ts(ts_path: Path, dictionary: dict[str, str], language: str) -> None:
    text = ts_path.read_text(encoding="utf-8")
    text = re.sub(r'language="[^"]+"', f'language="{language}"', text, count=1)

    def repl(match: re.Match[str]) -> str:
        block = match.group(0)
        sm = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
        if not sm:
            return block
        source = xml_unescape(sm.group(1))
        tr = dictionary.get(source, source)
        return re.sub(
            r"<translation[^>]*>.*?</translation>",
            f"<translation>{xml_escape(tr)}</translation>",
            block,
            count=1,
            flags=re.DOTALL,
        )

    out = re.sub(r"<message>.*?</message>", repl, text, flags=re.DOTALL)
    ts_path.write_text(out, encoding="utf-8")
    print(ts_path.name, "msgs", len(re.findall(r"<message>", out)))


# Fresh extract
ts = cp / "CookingProgress.ts"
if ts.exists():
    ts.unlink()

cmd = [
    str(QTBIN / "lupdate.exe"),
    ".",
    "../Forms/dlgsplashscreen.cpp",
    "../Forms/dlgsplashscreen.ui",
    "../Forms/c5connectiondialog.cpp",
    "../Forms/c5connectiondialog.ui",
    "../Cafe5/c5message.cpp",
    "-ts",
    "CookingProgress.ts",
    "-no-obsolete",
]
print(">", " ".join(cmd))
subprocess.run(cmd, cwd=str(cp), env=env, check=True)

hy: dict[str, str] = {}
hy.update(apply_existing(shop / "Shop.ts"))
hy.update(apply_existing(fd / "FrontDesk.ts"))
hy.update(json.loads((shop / "_hy_manual.json").read_text(encoding="utf-8")))
fill_ts(ts, hy, "hy_AM")

ru: dict[str, str] = {}
ru.update(json.loads((shop / "ru_dict.json").read_text(encoding="utf-8")))
ru.update(json.loads((fd / "ru_dict.json").read_text(encoding="utf-8")))

text = ts.read_text(encoding="utf-8").replace('language="hy_AM"', 'language="ru_RU"', 1)


def repl_ru(match: re.Match[str]) -> str:
    block = match.group(0)
    sm = re.search(r"<source>(.*?)</source>", block, re.DOTALL)
    if not sm:
        return block
    source = xml_unescape(sm.group(1))
    tr = ru.get(source, source)
    return re.sub(
        r"<translation[^>]*>.*?</translation>",
        f"<translation>{xml_escape(tr)}</translation>",
        block,
        count=1,
        flags=re.DOTALL,
    )


out = re.sub(r"<message>.*?</message>", repl_ru, text, flags=re.DOTALL)
(cp / "CookingProgress_ru.ts").write_text(out, encoding="utf-8")

subprocess.run(
    [str(QTBIN / "lrelease.exe"), "CookingProgress.ts", "-qm", "CookingProgress.qm"],
    cwd=str(cp),
    env=env,
    check=True,
)
subprocess.run(
    [str(QTBIN / "lrelease.exe"), "CookingProgress_ru.ts", "-qm", "CookingProgress_ru.qm"],
    cwd=str(cp),
    env=env,
    check=True,
)

# show sources
for line in re.findall(r"<source>(.*?)</source>", (cp / "CookingProgress.ts").read_text(encoding="utf-8")):
    print("-", xml_unescape(line))
print("ok")
