#!/usr/bin/env python3
import sys
import shutil
import re
from pathlib import Path

def process_file(path: Path):
    bak = path.with_suffix(path.suffix + '.bak')
    if not bak.exists():
        shutil.copy2(path, bak)
        print(f"Backed up {path} → {bak}")
    else:
        print(f"Backup exists: {bak}")

    changed = False
    pattern = re.compile(r'^(\s*)virtual\s+(.*\boverride\b.*)$')
    new_lines = []
    for lineno, line in enumerate(path.read_text().splitlines(), 1):
        m = pattern.match(line)
        if m:
            indent, rest = m.groups()
            new_line = f"{indent}{rest}"
            if new_line != line:
                print(f"{path}:{lineno}: removed redundant 'virtual'")
                line = new_line
                changed = True
        new_lines.append(line)

    if changed:
        path.write_text("\n".join(new_lines) + "\n")
        print(f"Updated {path}\n")
    else:
        print(f"No changes in {path}\n")

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <file-or-dir> [<file-or-dir> ...]", file=sys.stderr)
        sys.exit(1)

    for arg in sys.argv[1:]:
        p = Path(arg)
        if p.is_dir():
            for ext in ('*.h', '*.hpp'):
                for file in p.rglob(ext):
                    process_file(file)
        elif p.is_file():
            process_file(p)
        else:
            print(f"Warning: not found: {arg}", file=sys.stderr)

if __name__ == '__main__':
    main()
