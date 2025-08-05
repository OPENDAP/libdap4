
# Repair missing 'override' warnings

## Part 1

Given a but of goop from the compiler, extract files with missing 'override' keywords and get set to
automatically edit them.

Here’s a simple Python script you can drop into your PATH (or run in-place) that will scan through your CMake output file and pull out every warning of the form:

```
/path/to/File.h:LINE:COLUMN: warning: 'FUNCTION' overrides a member function but is not marked 'override'
```

and write out a tab-separated file of:

```
/full/path/to/File.h<TAB>FUNCTION
```

Save this as, say, `extract_override_warnings.py`:

```python
#!/usr/bin/env python3
import re
import sys

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_file> <output_file>", file=sys.stderr)
        sys.exit(1)

    infile, outfile = sys.argv[1], sys.argv[2]
    # regex breakdown:
    #   ^(.+?)         → group 1: shortest prefix up to first colon (the full path)
    #   :\d+:\d+:      → skip :LINE:COLUMN:
    #   \s+warning:\s+ → literal “ warning: ”
    #   '([^']+)'      → group 2: the function name inside quotes
    #   \s+overrides   → literal “ overrides” to anchor
    pattern = re.compile(
        r"^(.+?):\d+:\d+:\s+warning:\s+'([^']+)'\s+overrides.*not marked 'override'"
    )

    with open(infile, 'r') as fin, open(outfile, 'w') as fout:
        for line in fin:
            m = pattern.match(line)
            if m:
                path, func = m.group(1), m.group(2)
                fout.write(f"{path}\t{func}\n")

if __name__ == "__main__":
    main()
```

Make it executable:

```bash
chmod +x extract_override_warnings.py
```

Then run:

```bash
./extract_override_warnings.py build_output.txt override_list.txt
```

Each line in `override_list.txt` will look like:

```
/Users/jimg/src/opendap/hyrax/libdap4/Structure.h    ptr_duplicate
/Users/jimg/src/opendap/hyrax/libdap4/Structure.h    transform_to_dap4
...
```

You can then feed `override_list.txt` into whatever tool you like (awk, another Python script, etc.) to automatically insert `override` or make other fixes.

## PArt 2

Below is a standalone Python script, `apply_overrides.py`, that:

1. Reads a two-column list (`override_list.txt`) of `<header-path>\t<function_name>` (as produced by your extractor).
2. Groups all function names by header file.
3. For each header, makes a backup (`.bak`) and edits in-place:

   * Finds every `virtual ... functionName(...)[ const];`
   * Inserts `override` immediately before the trailing semicolon (after `const` if present).
   * Leaves other lines untouched.

```python
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
```

### How to use

1. Make it executable:

```bash
   chmod +x apply_overrides.py
```

2. Run it against your list:

```bash
   ./apply_overrides.py override_list.txt
```

This will:

* Create a `.bak` copy of each header before editing.
* Scan for each `virtual … functionName(...) [const];` and insert `override` if missing.
* Report each insertion with filename and line number.

You can tweak the regex if you have more complex declarations, but this should handle both plain and `const` methods, as well as multiple overloads in the same file.

## There's a part 3

Here’s a Python script, `remove_redundant_virtual.py`, that will recursively scan your headers (by default, `.h` and `.hpp` files) and remove any leading `virtual` on lines that already contain `override`. It makes a `.bak` backup of each file before editing.

```python
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
```

### How it works

1. **Backup**
   Before editing `Foo.h` it makes `Foo.h.bak` (unless already present).

2. **Pattern**
   It looks for lines matching:

   ```regex
   ^(\s*)virtual\s+(.*\boverride\b.*)$
   ```

   * Captures any leading whitespace,
   * ensures `override` appears later on the line,
   * then strips out the `virtual `.

3. **Usage**

   ```bash
   chmod +x remove_redundant_virtual.py
   ./remove_redundant_virtual.py path/to/my/project/include
   # or specify individual files:
   ./remove_redundant_virtual.py libdap4/Structure.h libdap4/BaseType.h
   ```

This will remove only those `virtual` keywords that are truly redundant (i.e., on the same line as `override`), leaving all other `virtual` declarations intact.
