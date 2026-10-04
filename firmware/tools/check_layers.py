#!/usr/bin/env python3
"""Check the AUTOSAR-style layering rules of the Nereid firmware.

Every quoted #include in project sources is resolved to the layer that owns the header
and checked against the allowed-dependency table below (docs/software/firmware_architecture.md).
The CMake target graph already prevents most violations at compile time; this script
also covers rules CMake cannot express (e.g. intra-BSW rules, FreeRTOS only in Os.c).

Usage: python3 firmware/tools/check_layers.py   (exit code 1 on violations)
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

FW = Path(__file__).resolve().parent.parent
INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.MULTILINE)

FREERTOS_HEADERS = {"FreeRTOS.h", "task.h", "queue.h", "semphr.h", "timers.h", "event_groups.h",
                    "stream_buffer.h", "message_buffer.h", "list.h", "portable.h", "FreeRTOSConfig.h"}

# Source layer -> header layers it may include. "app:self" = the SWC's own headers.
ALLOWED = {
    "app":      {"common", "lib", "rte", "app:self"},
    "lib":      {"common", "lib"},
    "rte":      {"common", "lib", "rte", "app", "services", "det", "ecual", "variant"},
    "mcal":     {"common", "mcal", "variant"},
    "ecual":    {"common", "lib", "mcal", "ecual", "det", "variant"},
    "services": {"common", "lib", "mcal", "ecual", "services", "det", "variant"},
    "port":     {"common", "lib", "mcal", "port", "vendor", "variant", "external", "freertos"},
    "variant":  {"common", "lib", "mcal", "ecual", "services", "det", "rte", "app", "port", "vendor", "variant"},
}

# Explicit, documented exceptions: (path suffix, header)
EXCEPTIONS = {
    ("bsw/services/Os/src/Os.c", "freertos"),          # the OS abstraction wraps FreeRTOS
    ("port/host/src/main_host.c", "services"),         # entry point calls EcuM_Init()
}


def build_header_index() -> dict[str, str]:
    index: dict[str, str] = {}
    for path in FW.rglob("*.h"):
        rel = path.relative_to(FW).as_posix()
        parts = rel.split("/")
        if parts[0] in ("third_party", "build") or rel.startswith("port/stm32f412/cubemx/"):
            continue
        if parts[0] == "common":
            layer = "common"
        elif parts[0] == "lib":
            layer = "lib"
        elif rel.startswith("bsw/mcal/"):
            layer = "mcal"
        elif rel.startswith("bsw/ecual/"):
            layer = "ecual"
        elif rel.startswith("bsw/services/Os/config/"):
            layer = "freertos"  # kernel configuration, consumed by the ports' FreeRTOSConfig.h
        elif rel.startswith("bsw/services/"):
            layer = "det" if path.name == "Det.h" else "services"
        elif parts[0] == "rte":
            layer = "rte"
        elif parts[0] == "app":
            layer = f"app:{parts[1]}"
        elif parts[0] == "variants":
            layer = "variant"
        elif parts[0] == "port":
            layer = "port"
        else:
            continue
        index[path.name] = layer
    return index


def source_layer(rel: str) -> tuple[str, str | None]:
    parts = rel.split("/")
    if parts[0] == "app":
        return "app", parts[1]
    if parts[0] == "lib":
        return "lib", None
    if parts[0] == "rte":
        return "rte", None
    if rel.startswith("bsw/mcal/"):
        return "mcal", None
    if rel.startswith("bsw/ecual/"):
        return "ecual", None
    if rel.startswith("bsw/services/"):
        return "services", None
    if parts[0] == "port":
        return "port", None
    if parts[0] == "variants":
        return "variant", None
    return "", None


def classify_header(name: str, index: dict[str, str]) -> str:
    if name in FREERTOS_HEADERS and name not in index:
        return "freertos"
    if name in index:
        return index[name]
    if name.startswith("stm32") or name == "main.h":
        return "vendor"
    return "external"


def main() -> int:
    index = build_header_index()
    violations: list[str] = []
    checked = 0

    for path in sorted(list(FW.rglob("*.c")) + list(FW.rglob("*.h"))):
        rel = path.relative_to(FW).as_posix()
        if rel.split("/")[0] in ("third_party", "build", "tests") or rel.startswith("port/stm32f412/cubemx/"):
            continue
        layer, swc = source_layer(rel)
        if not layer:
            continue
        checked += 1
        for header in INCLUDE_RE.findall(path.read_text(encoding="utf-8", errors="replace")):
            target = classify_header(header, index)
            if any(rel.endswith(p) and target == t for p, t in EXCEPTIONS):
                continue
            allowed = ALLOWED[layer]
            if target.startswith("app:"):
                ok = ("app:self" in allowed and target == f"app:{swc}") or "app" in allowed
            elif target == "external":
                ok = "external" in allowed or layer != "port"  # libc headers are fine everywhere
            else:
                ok = target in allowed
            if not ok:
                violations.append(f"{rel}: [{layer}] must not include \"{header}\" [{target}]")

    for v in violations:
        print(f"LAYER VIOLATION  {v}")
    print(f"check_layers: {checked} files checked, {len(violations)} violation(s)")
    return 1 if violations else 0


if __name__ == "__main__":
    sys.exit(main())
