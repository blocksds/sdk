# Invaders demo for BlocksDS

This example shows how to develop a simple game that loads all of its assets
from NitroFS. Graphics are created as PNG files, they are converted to GRF
format and they are loaded at runtime.

The build system used is [ArchitectDS](https://codeberg.org/blocksds/architectds)
because of the flexibility it gives the user.

In order to build this example, run:

```bash
python3 build.py
```

To clean the build results, run:

```bash
python3 build.py --clean
```
