# Fallout Malloc

Fallout Malloc is a silly recreation of the memory management system used in the original Fallout.
Due to a bug in the watcom compiler they had to do some interesting memory management to get the game running & I thought it'd be fun to recreate it (even though it's definetely not necassery in this day and age)


the inspiration came from this [video](https://www.youtube.com/watch?v=6kB_fko6SIg) By Tim Cain One of the original programmers


# How it Works

It is effectively an arena allocator so:
call malloc once requesting a big amount of memory
wrap that allocation in a framework of handlers
expose methods to request, lock, delete & resize memory
Squash the memory periodically to ensure you've always got space

# Other Features
## memory visualisation tool
the API can expose a socket which the test_viz.py can monitor and make a graph showing the layout of memory (WIP)

