#!/usr/bin/env python3
import json
import subprocess
import sys
from pathlib import Path

BIN = Path(__file__).resolve().parent / "main"

if not BIN.exists():
    print("Error: compiled main executable not found. Run 'make' first.")
    sys.exit(1)

result = subprocess.run([str(BIN), "--dump"], capture_output=True, text=True)
if result.returncode != 0:
    print("Error: main exited with non-zero status")
    print(result.stderr.strip())
    sys.exit(result.returncode)

try:
    data = json.loads(result.stdout)
except json.JSONDecodeError as exc:
    print("Error: failed to parse allocation dump:", exc)
    print("stdout:", result.stdout)
    sys.exit(1)

print("Allocated blocks:")
for index, alloc in enumerate(data.get("allocations", [])):
    print(f"  block {index}: offset={alloc['offset']}, size={alloc['size']}")
print(f"Top: {data.get('top')}")
print(f"Total memory capacity: {data.get('mem_total')}")
