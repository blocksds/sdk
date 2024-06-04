# GL2D: Draw background with a tileset

The map has been created with [Tiled](https://www.mapeditor.org/). You can find
the Tiled project files in the `tiled_project` folder.

The map has been exported as a `CSV` file, which has been copied into the source
code of the example. The dimensions of the map have also been added to the code
manually.

This method of drawing backgrounds is very inefficient. The DS has a limit of
6144 vertices, which means it can draw 1536 individual quads. In this image
tiles are 16x16 pixels, which means that we need this many quads to fill the
screen:

   ((256 / 16) + 1) * ((192 / 16) + 1) = 221

Note that the `+ 1` are required because when you scroll the map there are two
columns and two rows of tiles that are partially shown on the screen.

221 quads may not look like a lot, but sending the vertices to the GPU is very
slow compared to using a 2D background, which is free from the point of view of
the CPU.

## Information about `tiny-16.png`

Author:  FuwanekoGames
License: CC-BY 3.0
Source:  https://opengameart.org/content/tiny16-tileset
