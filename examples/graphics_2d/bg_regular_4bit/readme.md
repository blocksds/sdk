# Loading a 4 bpp background

This example shows how to load a 4 bpp background. It is a bit hard to generate
a valid image that can be converted to a 4 bpp background with grit, so this
example uses ptexconv.

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
