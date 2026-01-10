---
title: Scrolling big backgrounds
weight: 7
---

## 1. Introduction

As we have seen in the [backgrounds chapter](../backgrounds), the DS only
supports tiled 2D backgrounds with some specific map and tileset sizes:

Type            | Max. tileset size | Max. map size
----------------|-------------------|--------------
Regular         | 1024              | 512x512
Affine          | 256               | 1024x1024
Extended affine | 1024              | 1024x1024
8-bit bitmap    | N/A               | 512x512
16-bit bitmap   | N/A               | 512x512
Large bitmap    | N/A               | 1024x512 or 512x1024

If your backgrounds fit inside the supported sizes, you don't have to do
anything special. If your background is bigger than the hardware limits, you
need special code to load data to VRAM as needed. This will take some CPU time,
but it's the only way to get around the limitations of the hardware.

{{< callout type="warning" >}}
VRAM can only be written in 16-bit or 32-bit chunks. Be careful with affine
tiled backgrounds or 8-bit bitmaps.
{{< /callout >}}

The examples below work by editing map entries one by one. Entries in regular,
extended affine, and 16-bit bitmaps are 16 bit wide, so they work as expected.
Entries in affine and 8-bit bitmaps are 8 bit wide, so you need to take extra
care when editing them (read 16 bits, edit the value, then write the result). No
examples of this are shown in this tutorial.

## 2. Tiled backgrounds

### 2.1 Background with big map

This is the most common scenario. If you have a tileset that fits in a tileset
slot in VRAM but you have a map that is too big, you will need to load parts of
the map as you move around. This is very easy to understand with this example
that uses a regular tiled background:

<video controls>
  <source src="scroll_bigmap.webm" type="video/webm">
  Your browser does not support the video tag.
</video>

When the background is loaded the code only loads a small part of the map to
VRAM (the part that fits inside the screen viewport). As you scroll, the code
loads more rows or columns, one at a time. When the edge of the hardware
background is reached we just continue because regular tiled backgrounds wrap
around.

In the [backgrounds](../backgrounds) chapter we saw that regular tiled
backgrounds use a SSB layout and to convert them with grit with the `-mLs`
option. This is true if you're going to copy the image right away to VRAM, but
we aren't going to do that. We're going to read from the converted data and
write to VRAM manually. Reading the original data is easier if it's arranged as
a flat layout, so we use `-mLf` with grit. Writing to VRAM requires special care
because we will need to write it as a SSB layout.

Also, remember to ask grit (or your graphics conversion program) to optimize the
tileset as much as possible by flipping tiles, looking for repeated tiles, etc.
As long as the tileset fits in a VRAM tileset slot, it's ok.

If you're going to use this technique, all you need is a 512x256 background
layer. The resolution of the screen of the DS is 256x192, which corresponds to
32x24 tiles (8x8 pixels each). However, if you scroll pixel by pixel you will
sometimes see two partial rows and two partial columns around the edges of the
screen. This means that you need to reserve an additional row and column around
the screen, so you need a map with room for at least 33x25 tiles. That means
that the smallest size that you can use is 512x256.

You may have noticed that the red rectangle of the screen moves around the map
instead of staying around the top-left corner. This happens because the hardware
X and Y scroll registers are modified as the player moves around the map. You
could stay in one corner of the map, shift the tiles by one position once you
reach the next tile, and reset the scroll. However, that means that each new
row/column would force you to shift all the map. If you scroll the map as you
move around, you only ever have to load one row or column.

You can explore the code of the example here:
[`examples/graphics_2d/bg_scroll_bigmap`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/graphics_2d/bg_scroll_bigmap)

### 2.2 Background with big map and big tileset

This situation is a lot more complicated than just having a big map. If you have
a big tileset you need to load tiles to VRAM dynamically as you need them, and
you need to keep track of which ones are used so that you can free the unused
tiles and reuse that space to load other tiles as you scroll.

The general idea of this system is to:

- Convert the background using a custom format that allows bigger tile indices
  than the hardware formats. The hardware has a limit of 10 bits for the tile
  index, which lets you use up to 1024 tiles. If you use a custom format you can
  increase it to, for example, 16 bits, and be able to use up to 65536 tiles.

  If you're using grit, `-mB16:i16` will use a custom format where each tile map
  entry is a 16-bit index to a tile. You will need to disable optimizations
  based on tile flipping because this custom format doesn't support them. You
  can still try to deduplicate tiles that are exactly the same with `-mRt`.

- When you initialize the system, you load the palette and load just enough
  tiles to fit in the current viewport. As we saw in the previous section, you
  will need at most 825 (33x25) tiles loaded at once because that's all you
  need to fill the screen. You will need to keep track of which tiles indices
  are used in VRAM, and create a tile allocator based on that information. 825
  is smaller than 1024, so you will never run out of space in VRAM to load tiles
  as long as your code frees tiles correctly when they are no longer needed.

- When you load tiles to VRAM you need to keep track of their original 16-bit
  index and their current VRAM 10-bit index. You also need to keep track of how
  many times each VRAM entry is used. You can do this with an
  `std::unordered_map` that uses a 16-bit index as index. Each element should
  have a 10-bit index and a counter of how many times that tile is used.

- When moving around the map you need to load tiles of areas that are going to
  appear on the screen, but you also need to unload tiles of areas that leave
  the screen. You don't need to clear them in VRAM when you unload a tile, but
  you need to remember that they are free so that you can copy new tiles later.

The following two videos (recorded with no$gba) show how the engine works
internally. This video shows how tiles are loaded in real time:

<video controls>
  <source src="scroll_infinite_tileset.webm" type="video/webm">
  Your browser does not support the video tag.
</video>

And this video shows how the background is updated in real time. Note that the
parts of the background that are outside of the screen are corrupted as other
parts of the background are loaded, but that's ok because they won't ever be
shown:

<video controls>
  <source src="scroll_infinite_map.webm" type="video/webm">
  Your browser does not support the video tag.
</video>

You can explore the code of the example here:
[`examples/graphics_2d/bg_scroll_bigmap_bigtileset`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/graphics_2d/bg_scroll_bigmap_bigtileset)

This example lets you enable "debug" mode. The only difference is that it sets
VRAM data to 0 when it unloads it. Normally you don't need to do that because it
takes time and it doesn't do anything useful, but it can be very useful for
debugging the scroll engine with emulators that show the contents of VRAM.

## 3. Bitmap backgrounds

It's also possible to apply this technique to bitmap backgrounds, not just tiled
backgrounds. In this case, all you need to do is to load rows and columns of
pixels as they appear on the screen.

This example shows you how to scroll a 16-bit bitmap:
[`examples/graphics_2d/bg_scroll_big_bmp`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/graphics_2d/bg_scroll_big_bmp)

You could try to optimize the code by loading several rows/columns at the same
time, for example, but this example only tries to show how to use the technique
to scroll backgrounds, even if it's not the most optimal implementation.
