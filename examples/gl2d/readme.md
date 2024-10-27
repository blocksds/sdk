# BlocksDS SDK examples: GL2D

This folder contains examples of how to use GL2D, a library that uses the 3D
hardware to draw 2D images without the restrictions of the 2D hardware.

Note about sprite sets and tile sets: DS textures are restricted to sizes that
are powers of two. Sprite sets are images with a valid texture size that
contains many small images of arbitrary sizes so that memory is used
efficiently. Tile sets are similar, but all the images inside of the texture
must have the same dimensions and be laid out as a grid.

- `2d_and_3d`: Uses GL2D and regular 3D functions to draw 2D and 3D objects.
- `primitives`: It shows how to draw simple dots, lines and polygons.
- `sprite_transform`: Displays scaled and rotated sprites.
- `spriteset`: Displays a sprite set stored in the ARM9 as data.
- `spriteset_nitrofs`: Displays a sprite set stored in NitroFS.
- `tileset_background`: It displays a background made by a tile set.
- `tileset_sprites`: It displays sprites stored in a tile set.
