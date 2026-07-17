#!/usr/bin/env python3
#
# Diagnostic for the SonarScan coverage pipeline. jhrg 7/17/26
#
# gcovr's --sonarqube report (coverage.xml) lists every source file gcovr
# found real .gcno/.gcda data for. But Sonar's C-family analyzer only
# "knows about" (and therefore can attach coverage to) files that appear
# in the compile_commands.json produced by build-wrapper. If a file has
# real coverage data but never got captured by build-wrapper, Sonar will
# silently drop its coverage numbers -- no error, just a smaller
# denominator than gcovr reports.
#
# This script diffs the two file lists and reports anything in
# coverage.xml that is NOT in compile_commands.json, which is the set of
# files most likely to be invisible to Sonar's coverage import.
#
# Usage:
#   python3 travis/check-coverage-compile-commands.py coverage.xml bw-output/compile_commands.json [--root .]
#
# Exits 0 always (this is a diagnostic, not a build gate) unless a
# required input file is missing or unparsable, in which case it exits 1.

import argparse
import json
import os
import sys
import xml.etree.ElementTree as ET


def normalize(path, root):
    """Return a path relative to root, using forward slashes, with any
    leading './' stripped, so entries from both sources compare equally
    regardless of how each tool spelled the path."""
    if not os.path.isabs(path):
        # compile_commands.json entries are sometimes already relative to
        # `directory`, sometimes relative to the project root -- try to
        # resolve against root first since that's what coverage.xml uses.
        candidate = os.path.normpath(os.path.join(root, path))
    else:
        candidate = os.path.normpath(path)

    try:
        rel = os.path.relpath(candidate, root)
    except ValueError:
        # different drive on Windows, etc. -- fall back to the raw path
        rel = candidate

    rel = rel.replace(os.sep, "/")
    if rel.startswith("./"):
        rel = rel[2:]
    return rel


def load_coverage_files(coverage_xml_path, root):
    tree = ET.parse(coverage_xml_path)
    files = set()
    for file_el in tree.getroot().findall("file"):
        path = file_el.get("path")
        if path:
            files.add(normalize(path, root))
    return files


def load_compile_commands_files(compile_commands_path, root):
    with open(compile_commands_path) as f:
        entries = json.load(f)

    files = set()
    for entry in entries:
        file_field = entry.get("file")
        if not file_field:
            continue
        directory = entry.get("directory", root)
        if os.path.isabs(file_field):
            abs_path = file_field
        else:
            abs_path = os.path.join(directory, file_field)
        files.add(normalize(abs_path, root))
    return files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("coverage_xml", help="Path to gcovr's --sonarqube XML report")
    parser.add_argument("compile_commands", help="Path to build-wrapper's compile_commands.json")
    parser.add_argument("--root", default=".", help="Project root that both file lists should be normalized against (default: cwd)")
    parser.add_argument("--show-extra", action="store_true",
                         help="Also list files present in compile_commands.json but absent from coverage.xml (usually just test/tool sources gcovr excludes on purpose -- noisy, off by default)")
    args = parser.parse_args()

    root = os.path.abspath(args.root)

    if not os.path.isfile(args.coverage_xml):
        print(f"ERROR: coverage report not found: {args.coverage_xml}", file=sys.stderr)
        return 1
    if not os.path.isfile(args.compile_commands):
        print(f"ERROR: compile_commands.json not found: {args.compile_commands}", file=sys.stderr)
        return 1

    try:
        coverage_files = load_coverage_files(args.coverage_xml, root)
    except ET.ParseError as e:
        print(f"ERROR: could not parse {args.coverage_xml}: {e}", file=sys.stderr)
        return 1

    try:
        compile_files = load_compile_commands_files(args.compile_commands, root)
    except (json.JSONDecodeError, OSError) as e:
        print(f"ERROR: could not parse {args.compile_commands}: {e}", file=sys.stderr)
        return 1

    missing = sorted(coverage_files - compile_files)
    extra = sorted(compile_files - coverage_files)

    print(f"Files in {args.coverage_xml}: {len(coverage_files)}")
    print(f"Files in {args.compile_commands}: {len(compile_files)}")
    print()

    if missing:
        print(f"*** {len(missing)} file(s) have coverage data but are NOT in compile_commands.json ***")
        print("*** Sonar's C-family analyzer likely cannot attach coverage to these -- ***")
        print("*** they are the leading suspects for the gcovr-vs-Sonar percentage gap. ***")
        for path in missing:
            print(f"  MISSING: {path}")
    else:
        print("No gap found: every file with coverage data is present in compile_commands.json.")
        print("If Sonar's totals still come in lower than gcovr's, the compile-commands")
        print("theory is ruled out -- look at header multi-inclusion handling next.")

    if args.show_extra and extra:
        print()
        print(f"({len(extra)} file(s) in compile_commands.json but not in coverage.xml -- "
              f"expected for test/tool sources gcovr excludes on purpose)")
        for path in extra:
            print(f"  EXTRA: {path}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
