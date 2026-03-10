#!/usr/bin/env python3
import sys, subprocess, difflib, pathlib

if len(sys.argv)!=5 and len(sys.argv)!=3:
    print(f"Usage: {sys.argv[0]} <exe> <input.das> [<baseline> <out.diff>]", file=sys.stderr)
    sys.exit(1)

if len(sys.argv)==5:
    exe, input_path, baseline_path, out_diff = sys.argv[1:]
    inp = pathlib.Path(input_path).read_text()
    # run das-test
    proc = subprocess.run([exe, "-p"], input=inp.encode(),
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out_lines = proc.stdout.decode().splitlines()
    base_lines = pathlib.Path(baseline_path).read_text().splitlines()

    diff = list(difflib.unified_diff(
        base_lines, out_lines,
        fromfile=baseline_path, tofile=str(out_diff)
    ))
    if diff:
        # write unified diff for CTest logs
        pathlib.Path(out_diff).write_text("\n".join(diff))
        sys.exit(1)
elif len(sys.argv)==3:
    exe, input_path = sys.argv[1:]
    inp = pathlib.Path(input_path).read_text()
    proc = subprocess.run([exe, "-p"], input=inp.encode(),
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out_lines = proc.stdout.decode().splitlines()
    print(f"Output:")
    for line in out_lines:
        print(line)

sys.exit(0)
