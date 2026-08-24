# Converting images to use multiple extended palettes

This example shows how to convert a single image to use multiple extended
palettes. This isn't possible with grit, you need to install ptexconv:

```sh
wf-pacman -Sy blocksds-ptexconv
```

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
