# GL2D Sprite Set loading from NitroFS

This example shows how to load sprite sets from NitroFS to be used by GL2D. The
PNG files are packed into a single PNG file, which is then converted to GRF
format. In order to know where each sub-image is located in the final image,
a GUV file is generated. Both the GRF and GUV files need to be loaded at
runtime.

The build system used is [ArchitectDS](https://github.com/AntonioND/architectds)
because of the flexibility it gives the user.

In order to build this example, run:

```bash
python3 build.py
```

To clean the build results, run:

```bash
python3 build.py --clean
```
