# Background type loading from NitroFS

This example shows how to load background graphics from NitroFS. The PNG files
are converted to GRF format, which is loaded at runtime.

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

Note: It isn't possible to convert 4 bpp backgrounds with grit in an easy way
because the image must already have a pre-defined palette that works in 4 bpp
mode. This example uses ptexconv to convert an image to 4 bpp because ptexconv
doesn't require any specific palette in the image, it simply tries to generate
valid palettes for it.
