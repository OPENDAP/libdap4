#!/usr/bin/env bash
set -u

# Pass the test program (a C++ program) in so that cmake can use the correct
# program - it will not be installed in a directory on the PATH
exe="$1"
input="$2"
ce="$3"
baseline="$4"
output="$5"

# Optional: when set to 1/ON/TRUE, write baseline candidates instead of failing.
update="${UPDATE_BASELINES:-0}"

# Run the tool and capture output
"$exe" -C -x -e -t "$input" -c "$ce" > "$output" 2>&1

# Normalize output (for portability, don't use 'sed -i')
tmp="${output}_univ"
sed 's@<Value>[0-9a-f][0-9a-f]*</Value>@@' "$output" >"$tmp"
mv "$tmp" "$output"

# If asked to update baselines, write a candidate file for review.
# We write "<baseline>.new" next to the committed baseline.
shopt -s nocasematch || true
if [[ "$update" == "1" || "$update" == "on" || "$update" == "true" || "$update" == "yes" ]]; then
  new_baseline="${baseline}.new"
  mkdir -p "$(dirname "$new_baseline")"
  cp -f "$output" "$new_baseline"

  echo "Wrote baseline candidate: $new_baseline" >&2

  rm -f "$output"
  exit 0
fi
shopt -u nocasematch || true

# Normal mode: compare and return diff's exit code
diff -b -B "$baseline" "$output"
rc=$?

rm -f "$output"
exit $rc
