from pathlib import Path
import re
from pypdf import PdfReader

pdf = Path(__file__).with_name("uc_hhpek_hdm_integration_manual_2025_v073_arm+.pdf")
out = Path(__file__).with_name("_pdf_mode_out.txt")
text = ""
for p in PdfReader(str(pdf)).pages:
    text += (p.extract_text() or "") + "\n"

ascii_lines = []
for line in text.splitlines():
    if re.search(r"mode|items|null|dep", line, re.I):
        clean = line.encode("ascii", "replace").decode("ascii")
        if clean.strip():
            ascii_lines.append(clean.strip())

# also pull JSON-like blocks
for m in re.finditer(r"\{[^{}]{20,400}\}", text):
    block = m.group(0)
    if "mode" in block.lower():
        ascii_lines.append(block.encode("ascii", "replace").decode("ascii"))

out.write_text("\n".join(dict.fromkeys(ascii_lines)), encoding="utf-8")
print("lines", len(ascii_lines))
