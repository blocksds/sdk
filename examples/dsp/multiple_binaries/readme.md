# Multiple DSP binaries example

This example shows how to have multiple DSP binaries in the same ROM. One of
them is stored in NitroFS and the other one is stored as ARM9 data.

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
