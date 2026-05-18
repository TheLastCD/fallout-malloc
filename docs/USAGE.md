# Usage & Developer Guide

This project contains a tiny arena-style allocator and utilities. This guide covers common developer workflows.

Building

- Build the library and main program:

  make

- Build and run tests:

  make test
  ./test_runner

Tests cover:

| Test name | Covers |
|-----------|--------|
| `run_test_alloc` | basic `Block` allocation, write/read, and deletion |
| `run_test_delete_squash` | delete a middle allocation, call `squash()`, and verify moved blocks still contain data |
| `run_test_request_invalid_size` | invalid `request_memory()` sizes are rejected |
| `run_test_zero_size_block` | zero-size `Block` requests fail safely |
| `run_test_delete_unallocated` | deleting an unallocated block returns failure |
| `run_test_lock_unlock` | lock/unlock command semantics and error cases |
| `run_test_squash_empty` | `squash()` on an empty allocator is safe |
| `run_test_squash_with_locked_block` | locked blocks act as immovable barriers during compaction |

- Build the demo (server enabled) and run it:

  make demo SERVER=1
  ./demo

  The demo starts a TCP server on `127.0.0.1:9999` which streams JSON lines describing the memory layout. Use `utils/mem_viz.py` to visualize.

Python visualizer

- Install dependencies (matplotlib):

  pip3 install matplotlib

- Run visualizer and connect to the demo server:

  python3 utils/mem_viz.py 9999

API notes

- See `docs/API.md` for the allocator API and usage patterns.

Extending and debugging

- To enable allocation dumps for diagnostics build with `make dump` (sets `DUMP=1`). Use `utils/show_allocations.py` to parse output from `dump_allocations()`.
- For live server support, compile with `SERVER=1`.
