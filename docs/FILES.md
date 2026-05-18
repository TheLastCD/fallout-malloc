# Codebase File Reference

This document summarizes the purpose of each file in the repository and the important symbols to look for.

Top-level
- `README.md` — high-level project description and inspiration.
- `makefile` — build rules for library, main, tests, and demo. Use `make`, `make test`, or `make demo SERVER=1`.

Includes (headers)
- `include/block.h` — `Block` struct definition and block helpers: `init_block()`, `make_memory_request()`, `del_block()`.
- `include/lock.h` — lock API: `lock_block()`, `unlock_block()` and error codes.
- `include/memspace.h` — allocator public API: `init_memspace()`, `free_memspace()`, `request_memory()`, `allocate_memory()`, `squash()`, plus `register_block()`/`unregister_block()` used internally. Server control APIs are available when compiled with `-DENABLE_MEMSPACE_SERVER`.

Source (`src/`)
- `src/memspace.c` — core allocator implementation.
  - Global state: `mem`, `mem_total`, `top`.
  - Allocation: `request_memory(size)` (public), `allocate_memory(size)` (bump-pointer).
  - Registry: `register_block(Block *)` and `unregister_block(Block *)` used so compaction can update `Block.handler_ptr`.
  - Compaction: `squash()` moves unlocked `Block` allocations to remove gaps and updates `Block.handler_ptr` accordingly.
  - (Optional) Live server: when built with `-DENABLE_MEMSPACE_SERVER` (via `make SERVER=1`), the file contains a background TCP server that streams JSON memory layout to clients.

- `src/block.c` — helpers for `Block` objects.
  - `init_block(int size)` creates a `Block` struct.
  - `make_memory_request(Block *blk)` requests memory and registers the `Block`.
  - `del_block(Block *blk)` unregisters and clears the `Block`.

- `src/lock.c` — simple lock helpers operating on `Block.locked`.

- `src/memspace_diagnostics.c` — optional allocation dump code (enabled with `DUMP=1` during build). Records allocations for debugging and provides `dump_allocations()`.

- `src/memstack.c` — placeholder (empty) file reserved for stack-like allocation helpers.

Tests (`tests/`)
- `tests/test_alloc.c` — verifies basic allocation via `Block` API.
- `tests/squash/test_delete_squash.c` — tests deletion + `squash()` compaction behavior.
- `tests/runner.c` — test runner that calls individual test functions.

Utilities
- `utils/show_allocations.py` — helper to parse diagnostic dumps (if `ENABLE_ALLOC_DUMP` was used) and show allocations.
- `utils/mem_viz.py` — live visualizer that connects to the memspace server and displays allocation layout using `matplotlib`.

Examples
- `examples/demo.c` — small demo program that starts the memspace server (if compiled with `SERVER=1`) and performs allocations/frees so the visualizer can show activity.

Docs
- `docs/API.md` — describes the public allocator API and usage patterns.
- `docs/FILES.md` — (this file) quick reference for repository files.

If you want more detailed docs for a particular file or function (e.g. annotate `memspace.c` with doc comments), tell me which files to flesh out next and I will add per-function documentation in-place.
