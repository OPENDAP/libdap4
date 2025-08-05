#!/usr/bin/env python3
import sys
import re
import shutil
from pathlib import Path
from collections import defaultdict

def load_overrides(list_file):
    """
    Read the override list file of lines:
      /path/to/File.h<TAB>functionName
    Returns a dict: { Path: set(functionName, ...) }
    """
    overrides = defaultdict(set)
    with open(list_file, 'r') as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line or '\t' not in line:
                continue
            path_str, func = line.split('\t', 1)
            path = Path(path_str)
            overrides[path].add(func)
    return overrides

def apply_override_to_file(path, funcs):
    """
    For each virtual declaration of functions in `funcs`, insert 'override' before ';'.
    Creates a backup at path.with_suffix(path.suffix + '.bak').
    """
    bak = path.with_suffix(path.suffix + '.bak')
    if not bak.exists():
        shutil.copy2(path, bak)
        print(f"Backed up {path} → {bak}")
    else:
        print(f"Backup already exists: {bak}")

    # Compile one regex per function for performance
    patterns = []
    for func in funcs:
        # pattern explanation:
        #   ^(?P<before>\s*virtual\b[^\n]*\bfunc\s*\([^;]*\))
        #       captures "    virtual ... func(...)" including trailing "const" if present
        #   (?P<const>\s+const)?    optionally capture " const"
        #   (?P<after>\s*;)$        capture the semicolon and EOL
        pat = re.compile(
            rf'^(?P<before>\s*virtual\b[^\n]*\b{re.escape(func)}\s*\([^;]*?\))'
            rf'(?P<const>\s+const)?'
            rf'(?P<after>\s*;)\s*$'
        )
        patterns.append((func, pat))

    # Read & rewrite
    changed = False
    lines = path.read_text().splitlines()
    out_lines = []
    for i, line in enumerate(lines, 1):
        new_line = line
        for func, pat in patterns:
            m = pat.match(line)
            if m:
                before = m.group('before')
                const = m.group('const') or ''
                after = m.group('after')
                # check if already has override
                if 'override' not in line:
                    new_line = f"{before}{const} override{after}"
                    print(f"{path}:{i}: added override to `{func}`")
                    changed = True
                break
        out_lines.append(new_line)

    if changed:
        path.write_text("\n".join(out_lines) + "\n")
        print(f"Updated {path}")
    else:
        print(f"No changes in {path}")

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} override_list.txt", file=sys.stderr)
        sys.exit(1)

    list_file = Path(sys.argv[1])
    if not list_file.exists():
        print(f"Error: {list_file} not found", file=sys.stderr)
        sys.exit(1)

    overrides = load_overrides(list_file)
    for path, funcs in overrides.items():
        if not path.exists():
            print(f"Warning: file not found: {path}", file=sys.stderr)
            continue
        apply_override_to_file(path, funcs)

if __name__ == "__main__":
    main()
