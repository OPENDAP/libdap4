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
