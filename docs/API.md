# Memory allocator API

This document describes the simple allocator API provided by this project.

Initialization

- `init_memspace(int block_size)` — initialize the allocator with `block_size` units. Returns `0` on success.
- `free_memspace()` — free internal memory and reset allocator state.

Allocation

- `int *request_memory(int size)` — allocate `size` units and return a pointer into the allocator's memory. Returns `NULL` if allocation fails.
- `int *allocate_memory(int size)` — internal bump-pointer allocation; prefer `request_memory`.

Block helpers

This project stores allocation owners in `Block` structs (see `include/block.h`). Use these helpers when working with `Block`:

- `Block init_block(int size)` — create a `Block` descriptor for an allocation of `size` units.
- `int make_memory_request(Block *blk)` — allocate memory for `blk` and register the `Block` so that allocator operations (like compaction) can update `blk->handler_ptr` if the allocation is moved.
- `int del_block(Block *blk)` — delete/unregister the `Block`, clear its memory, and trim internal `top` if the block was at the end.

Registry & compaction

- `int register_block(Block *blk)` — internal: register a `Block` with the memspace so compaction can update pointers.
- `int unregister_block(Block *blk)` — internal: remove a `Block` from the registry and free/trim its memory if possible.
- `int squash()` — compact allocations by moving unlocked (movable) `Block` allocations to remove gaps. The function updates each registered `Block`'s `handler_ptr` to point to the new location. Locked blocks (where `Block.locked` is non-zero) are treated as immovable barriers.

Locking

- `int lock_block(Block *blk)` / `int unlock_block(Block *blk)` — mark a block as locked to prevent compaction from moving it.

Notes & best practices

- Compaction (`squash`) relies on all live allocations being tracked via `Block` structs and registered with `register_block`. If user code stores raw pointers elsewhere (not in `Block.handler_ptr`), compaction will leave them invalid.
- Prefer to `del_block` blocks you no longer need rather than relying on implicit garbage collection.

Example

1. Create a block and request memory:

   Block b = init_block(16);
   make_memory_request(&b);

2. Use the memory through `b.handler_ptr`.

3. When done:

   del_block(&b);

   Live demo

   Build and run the included demo program which starts the server and performs allocations:

   - Build (enable server):

      make demo SERVER=1

   - Run the demo (starts server on port 9999 by default):

      ./demo

   - Run the visualizer (in another terminal):

      python3 utils/mem_viz.py 9999

   The demo will perform allocations, frees and periodic `squash()` calls; the visualizer shows the current memory layout in real time.
