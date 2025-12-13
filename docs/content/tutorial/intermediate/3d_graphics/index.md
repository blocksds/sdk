---
title: 3D graphics
weight: 9
---

## 1. Introduction

The Nintendo DS has a fairly powerful 3D engine for the size of the console.
libnds contains an OpenGL-like 3D library to use this engine. It isn't real
OpenGL 1 or anything like that. It's just an API that is similar to OpenGL 1 so
that it feels a bit more natural for people already used to it, but it has
nothing in common with modern OpenGL.

While it may look difficult at first, the 3D graphics engine of the Nintendo DS
is, in many ways, easier to use than the 2D graphic engines:

- You only need to configure your VRAM banks as "texture VRAM" and load your
  graphics data anywhere inside them. You don't need to worry about loading the
  right data to background or sprite VRAM, or about tile set and tile map slots
  for backgrounds.
- You don't need to worry about normal vs extended palettes, or about how
  palettes can be used in 16x16 color mode vs 256 color mode. All 3D texture
  palettes are treated the same way.
- There is a limit of 6144 vertices per frame. You can draw up to 2048 triangles
  or 1536 quads, which is a lot for the resolution of the screen.
- You can blend polygons with a lot more freedom than the 2D hardware blending
  registers. You can change the color of textured polygons so that they get
  drawn as if there was a colored filter in front of them.

Also, remember that you can use the 3D engine to render 2D graphics without the
limitations of the 2D graphics engines. We have seen an easy way to do it in the
[GL2D chapter](../gl2d), but you will learn how to do it by hand in this
chapter.

This chapter isn't meant to be a guide about every last detail of the 3D
hardware. If you want to know all of its secrets, check
[GBATEK](https://problemkaputt.de/gbatek.htm#ds3dvideo).

Also, as mentioned in the GL2D chapter, the main limitation of the 3D engine is
that it can only be used in one screen. It is possible to display 3D on both
screens by sacrificing half of the FPS of the application, which will be
explained in detail in a future chapter. In case you can't wait, there is an
example of how this system works here
[`examples/video_capture/dual_screen_3d`](https://github.com/blocksds/sdk/tree/master/examples/video_capture/dual_screen_3d).

## 2. Fixed point types

The GPU uses several types of fixed point for different tasks, and there are
some definitions in libnds to help you convert between floating point, integer
values and fixed point:

- `fixed12d3`: Used to represent depth distances (12.3).
- `t16`: Used to represent texture coordinates (12.4).
- `v16`: Used to represent vertices (4.12).
- `v10`: USed to represent normals or vertices (0.10).
- `f32`: Used to represent matrix componets (20.12).

The values inside the parentheses represent the number of bits used as integer
and fractionary parts. For example, `v16` is a signed type that goes from
`0x8000` to `0x7FFF`. This corresponds to `-0x8000 / (1 << 12)` and `0x7FFF /
(1 << 12)` in floating point, which is -8.0 to aprox. 7.99976. Values outside of
this range can't be represented in this format.

If you want a more in-depth explanation, check the chapter about fixed points in
Tonc [here](https://gbadev.net/tonc/fixed.html).

It can be hard to remember which type to use in each function, so most functions
are named after the type of fixed point type required. For example,
`glVertex3v16()` takes three `v16` values as arguments but `glVertex3f()` takes
three floats (that are converted internally to `v16`).

However, you should avoid using floating point values in your game. They can be
convenient to test the code initially, but calculations with floats are very
slow. If you use floating point constants the compiler can optimize calculations
and store only the final result.

For example, if you call `glVertex3f(1.0, 2.0, 3.0)` the compiler will see the
implementation of the function:

```c
static inline void glVertex3f(float x, float y, float z)
{
    glVertex3v16(floattov16(x), floattov16(y), floattov16(z));
}
```

It will see that the result of `floattov16()` is a constant in all three cases
and it will replace the original function call by `glVertex3v16(1 << 12, 2 <<
12, 3 << 12)`. The resulting code is just as efficient, but easier to read.

However, the following code probably won't be optimized and it will try to
convert between floating point and fixed point every time:

```c
float x = 0.0;

while (1)
{
    scanKeys();
    u16 keys = keysHeld();
    if (keys & KEY_LEFT)
        x -= 1.0;
    if (keys & KEY_RIGHT)
        x += 1.0;

    ...

    glVertex3f(x, 2.0, 3.0);

    ...
}
```

In general, you can start with floating point variables when you're learning and
you can switch to fixed point when you want to do something more complicated
that requires a more efficient CPU usage.

## 3. Initialization

A typical program that uses 3D looks like this:

```c
#include <nds.h>

int main(int argc, char *argv[])
{
    // Set any video mode that you want, but enable 3D
    videoSetMode(MODE_0_3D);

    // Initialize GL library
    glInit();

    // Initialize antialiasing. This will soften the edges of the polygons,
    // which is very noticeable on the screen of the DS due to its small
    // resolution.
    glEnable(GL_ANTIALIAS);

    // Setup the 3D rear plane. This plane can have any color that you want, and
    // it can be transparent or opaque. The background must be fully opaque and
    // have a unique polygon ID (different from the polygons that are going to
    // be drawn) so that antialiasing works.
    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    // Set the maximum distance at which polygons can be drawn
    glClearDepth(0x7FFF);

    // Set the full screen as the viewport for 3D graphics
    glViewport(0, 0, 255, 191);

    // Set the projection (camera) matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up direction

    // Switch to model view matrix
    glMatrixMode(GL_MODELVIEW);

    while(1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Send commands to the GPU here

        // Render frame
        glFlush(0);
    }

    // Deinitialize 3D hardware and free all memory used by videoGL
    glDeinit();
}
```

A downside of using the 3D engine is that you need to tell the GPU what to draw
every frame. With the 2D hardware you don't have to do anything once you've
configured your background and sprites unless something changes and, when
something changes, all you need to do is reconfigure the specific thing that has
changed. With the 3D hardware you need to send all the polygons to the GPU every
frame, which takes some CPU time. At the end of this chapter we will learn how
to use display lists, which is one of the ways to optimize 3D rendering.

## 4. Rendering a polygon

The first thing we need to do is to get a single polygon. Starting from the
example in the previous section, we can replace the contents of the main loop by
this:

```c
    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Draw scene

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        glBegin(GL_QUADS);

            glColor(RGB15(31, 0, 0));
            glVertex3v16(floattov16(-1), floattov16(-1), 0);

            glColor(RGB15(31, 31, 0));
            glVertex3v16(floattov16(1), floattov16(-1), 0);

            glColor(RGB15(0, 31, 0));
            glVertex3v16(floattov16(1), floattov16(1), 0);

            glColor(RGB15(0, 0, 31));
            glVertex3v16(floattov16(-1), floattov16(1), 0);

        glEnd();

        // Tell the GPU to use the new scene data in the next frame
        glFlush(0);
    }
```

![Colored polygon](colored_polygon.png)

You can check the source code of the example here (note that it uses some
functions we haven't seen in this chapter yet!):
[`examples/graphics_3d/colored_polygon`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/colored_polygon)

In this example we're drawing a single quad in which each vertex has a different
color. If you want to see how it looks you can build the code yourself (or you
can wait for the next section of this chapter!).

Function `glPolyFmt()` defines polygon attributes for anything that comes after
it. In this case we're setting:

- Alpha value: A value of 31 in `POLY_ALPHA()` means that polygons are fully
  opaque. A value of 0, however, means that polygons are drawn in wireframe mode
  (we will see an example of this later). Normally you will use values 1-31 for
  different levels of opacity, and you will simply avoid drawing any polygon
  that you don't want to draw (instead of setting the alpha to 0). We will see
  how to use alpha blending properly later.

- Culling: In order to improve performance the GPU can drop polygons that don't
  face the camera. For example, if you're drawing an opaque box, there will
  always be some polygons that you can't see because they are covered by others.
  If you enable back culling, the GPU will ignore them instead of wasting time
  trying to draw them and realizing that there are other polygons in front of
  them. In this example, for simplicity, we are disabling culling with
  `POLY_CULL_NONE`. We will experiment with culling later.

Polygons are drawn by sending a `glBegin()` command, calling some version of
`glVertex*()` and other related functions, and finishing with `glEnd()`. In this
case we're going to draw a quad, so we use `GL_QUADS`. `glColor()` sets the
color for the vertices drawn after the command, and `glVertex3v16()` sends new
vertices to the GPU with the 3 provided coordinates.

When all polygons have been sent to the GPU we call `glFlush()` so that the GPU
starts working on them for the next frame.

## 5. Moving the camera

The "camera" of your game is implemented in hardware as transformation matrices.
The matrices are stored in the 3D hardware, and all calculations done to them
are accelerated by hardware.

Normally you will use `glMatrixMode(GL_PROJECTION)` to setup you global
transformations (your camera), and `glMatrixMode(GL_MODELVIEW)` to setup the
transformation of a specific model. While in `GL_MODELVIEW` mode you will
transform the matrix several times, going back an forth to setup the
transformations for each one of your models.

You can easily transform your view with functions like `glTranslatef32()`,
`glScalef32()`, `glRotateX()`, etc. Check the documentation of libnds
[here](../../../libnds/videoGL_8h.html) for the full list of functions.

Models are usually drawn with vertices around the center of coordinates. For
example, you wouldn't move a box by adding the position of the box to the
coordinates of all the vertices of the box. You would always draw the box around
coordinates (0, 0, 0) and use translation commands to adjust the coordinates.

`glPushMatrix()` can save your current transformations in the stack for later,
and you can use `glPopMatrix()` later to restore the previous transformations
quickly.

With that knowledge you now know about all the functions used in the previous
example: [`examples/graphics_3d/colored_polygon`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/colored_polygon)

This tutorial doesn't have any example of a complex room being drawn, but it's
still important to understand the idea. For example, imagine you're inside a
room. You have:

- Walls, ceiling, floor, maybe fixed furniture like a wardrobe.
- Table, which can be moved by the player, with some objects on top.
- Chair, which can also be moved by the player, with other different objects.

You could do something like this:

```c
while (1);
{
    // Setup the camera to match the position of your player and the direction
    // it's looking at.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Reset matrix
    setup_camera(); // Set the camera

    // Start drawing models
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Now you can draw the floor, walls, ceiling, etc of the room
    draw_world();

    // We're going to draw the table, so we preserve the current transformations
    glPushMatrix();

    {
        move_to_table_position();
        draw_table();

        move_to_table_top();

        for (int i = 0; i < num_objects; i++)
        {
            glPushMatrix();

                move_to_object(i);
                draw_objects(i);

            glPopMatrix(1);
        }
    }

    // Restore the previous transformations.
    glPopMatrix(1);

    glPushMatrix();

    {
        move_to_chair_position();
        draw_chair();

        move_to_chair_top();

        for (int i = 0; i < num_objects; i++)
        {
            glPushMatrix();

                move_to_object(i);
                draw_objects(i);

            glPopMatrix(1);
        }
    }

    glPopMatrix(1);

    // Draw everything
    glFlush(0);
}
```

## 6. Culling

When you draw polygons you send the vertices to the GPU in a specific order.
This order is used by the GPU to decide whether the polygon is facing the camera
or away from it. If vertices are counter-clockwise when rendered on the screen
the GPU considers the polygon to be facing the camera.

Example [`examples/graphics_3d/basic_cube`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/basic_cube)
shows the effects of using back culling (on the left) or front culling (on the
right). Back culling shows the model as expected because it doesn't draw the
polygons that you don't normally see. Front culling hides the polygons you
normally see:

![Culling](culling.png)

Normally you won't be defining polygons by hand, so you don't have to worry
about the order of the polygons. Your 3D modelling program will export vertices
in the right format.

## 7. Using textures

Textures can be used to apply images to your polygons like in this example:

![Texture with no color](texture_nocolor.png)

Essentially, you need to:

- Convert your files to the right format.
- Assign some VRAM memory for 3D textures and 3D texture palettes.
- Load the textures.
- Before drawing textured polygons, tell the GPU to use that texture.
- While drawing textured polygons, send texture coordinates to the GPU
  interleaved with the vertices of the model.

The screenshot shown before corresponds to the example
[`examples/graphics_3d/basic_texture`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/basic_texture).
Let's see what has changed compared to the previous examples.

1. We can see that there is an image in the `graphics` folder, and the
   corresponding `.grit` file contains the following parameters:

   ```sh
   # 16 bit texture bitmap, force alpha bit to 1
   -gx -gb -gB16 -gT!
   ```

   This generates a 16-bit texture in RGBA format. This is the easiest texture type
   to load because it doesn't need a palette. We will see how to use paletted
   textures later.

2. Enable textures and assign some VRAM memory to be used for textures.

   ```c
   glEnable(GL_TEXTURE_2D);

   vramSetBankA(VRAM_A_TEXTURE);
   ```

   Only VRAM banks A to D can be used to store textures. The size of the banks
   is 128 KiB. This is easier than the VRAM modes for 2D graphics because there
   is only one mode for 3D textures (and one mode for 3D palettes, as we will
   see later). To load a texture you need to do the following:

3. You need an `int` that will contain a texture ID (called name in the videoGL
   documentation) that you need to use to interact with videoGL when you want to
   use or delete the texture.

   ```c
   int textureID;
   ```

4. Generate a texture ID. This function allocates a struct that contains texture
   metadata, but it doesn't allocate any texture data yet.  You can generate
   multiple IDs with a single call by providing an array of integers and passing
   the number of integers in the first argument.

   ```c
   if (glGenTextures(1, &textureID) == 0)
   {
       printf("Failed to generate ID\n");
       while (1)
           swiWaitForVBlank();
   }
   ```

5. Now that we have a valid ID, tell videoGL that this texture is the active
   texture we are going to use.

   ```c
   glBindTexture(0, textureID);
   ```

6. Finally, try to load it. This function tries to load a texture to the active
   texture ID. It can fail if there isn't enough memory or there is no active
   texture. This function will look for all VRAM banks assigned as texture VRAM
   and it will try to allocate the data there. It will ignore banks assigned to
   other types of data.

   ```c
   if (glTexImage2D(
           0, 0,     // Unused parameters (for compatibility with OpenGL)
           GL_RGBA,  // Texture format
           128, 128, // Texture size (width, height)
           0,        // Unused parameter (for compatibility with OpenGL)
           TEXGEN_TEXCOORD,  // Texture parameters (explained later)
           neonBitmap) == 0) // Pointer to the data
   {
       printf("Failed to load texture\n");
       while (1)
           swiWaitForVBlank();
   }
   ```

   `glTexImage2D()` in libnds has 3 unused parameters because the OpenGL version
   of this image uses them for things that aren't supported on DS. The
   parameters have been kept in the function prototype to help porting OpenGL
   code to libnds.

{{< callout type="warning" >}}
While you're copying 3D textures or palettes to VRAM the GPU doesn't have access
to them. If the GPU tries to access textures at this time it will read white
pixels. You can try to load textures in the vertical blanking period, but it
isn't very reliable. We will see later how to load textures on the fly safely.
For now, only load textures during loading screens in your application or game.
{{< /callout >}}

Most of the time you will use multiple textures, so you need to remember to bind
textures when you want to use them. In general, you need to do something like
this to draw textured polygons:

```c
glBindTexture(0, textureID);

// For now, set the color to white so that we can see the plain texture clearly
glColor3f(1, 1, 1);

glBegin(GL_QUADS);

    glTexCoord2t16(0, inttot16(128)); // Texture coordinates for the next vertex
    glVertex3v16(floattov16(-1), floattov16(-1), 0);

    glTexCoord2t16(inttot16(128), inttot16(128));
    glVertex3v16(floattov16(1), floattov16(-1), 0);

    // ...

glEnd();
```

You can delete textures with this function (it can also take an array of
integers like `glGenTextures()`):

```c
glDeleteTextures(1, &textureID);
```

And you can delete all loaded textures with:

```c
glResetTextures();
```

As a final note, you can temporarily disable textures by doing this:

```c
glBindTexture(0, NULL);
```

After doing that, all polygons are drawn as if there was a white texture active.
You can bind a texture at any point later when you're ready to use textures
again.

{{< callout type="warning" >}}
Sometimes you will notice that textures are drawn half a pixel or a pixel off.
This is unavoidable, and it happens because of inaccuracies in the calculations
in the GPU.
{{< /callout >}}

Note that you can change the color and the texture coordinates before every
vertex to achieve nicer graphics if you want. The last example lets you see this
effect if you hold button A. This is the result:

![Texture with color](texture_color.png)

## 8. Texture formats

The DS supports 7 texture formats, and 6 of them use palettes. Now that we know
how to load and use textures that don't need palettes we can start to think
about formats that require palettes.

- `GL_RGBA`: 16 bit textures: 5 bits for each one of the RGB components, 1 bit
  for alpha (2 bytes per pixel). It doesn't use palettes.

  You may see some code that uses `GL_RGB`. It is the same format internally,
  but `glTexImage2D()` sets the alpha bit to 1 internally (which takes CPU
  time!). This alias can be useful if your graphics conversion program doesn't
  set the alpha bit correctly, but it's generally better to set it to 1 manually
  before calling `glTexImage2D()`,

  Sample grit arguments:

  ```sh
  # 16 bit texture bitmap, force alpha bit to 1
  -gx -gb -gB16 -gT!
  ```

  ```sh
  # 16 bit texture bitmap, set magenta as transparent color
  -gx -gb -gB16 -gTFF00FF
  ```

- `GL_RGB256`, `GL_RGB16` `GL_RGB4`: Textures with 256, 16 or 4 color palettes.
  Each color in the palette uses 5 bits for each one of the RGB components (but
  the last bit isn't the alpha bit!). Option `GL_TEXTURE_COLOR0_TRANSPARENT`
  lets you define index 0 as transparent or not (we'll see how to later). The
  formats use 1 byte per pixel, 4 bits per pixel, and 2 bits per pixel
  respectively.

  Sample grit arguments:

  ```sh
  # 256-color (8 bits per pixel) texture, set magenta as transparent color
  -gx -gb -gB8 -gTFF00FF
  ```

  ```sh
  # 16-color (4 bits per pixel) texture, set black as transparent color
  -gx -gb -gB4 -gT000000
  ```

  ```sh
  # 4-color (2 bits per pixel) texture, set magenta as transparent color
  -gx -gb -gB2 -gTFF00FF
  ```

- `GL_RGB32_A3` and `GL_RGB8_A5`: They are translucent palettes in which you're
  allowed to set a different translucency value (alpha value) per pixel. They
  have 32 color and 8 color palettes respecively. This only uses 5 and 3 bits
  of a byte, and the remaining bits are used as alpha value for that pixel. We
  will see how to use the two formats later when we discuss alpha blending.
  Alpha blending requires some special handling (regular on/off transparency,
  like in the other formats, doesn't need any special handling).

  Sample grit arguments:

  ```sh
  # 5 bits of alpha, 3 bits of color index
  -gx -gb -gBa5i3 -gT!
  ```

  ```sh
  # 3 bits of alpha, 5 bits of color index
  -gx -gb -gBa3i5 -gT!
  ```

- `GL_COMPRESSED`: This is a special format. It splits the texture into blocks
  of 4x4 pixels and compresses the texture by sharing textures between different
  4x4 blocks. The actual details of the format aren't important, but you can see
  them [here](https://problemkaputt.de/gbatek.htm#ds3dtextureformats) if you're
  interested. This format is also called tex4x4 in some tools.

  This format uses a fixed amount of space for the texture itself (3 bits per
  pixel of texture memory) plus a variable amount of palette memory. The palette
  is where conversion tools can try to save space. Also, pixels can be made
  transparent.

  This format isn't good for textures that have a lot of details. Think of it as
  the JPEG format of the DS. Use it for natural-looking textures.

  You need to use ptexconv to convert images to this format, grit doesn't
  support it. Check [this link](../../../docs/libs/tools) for information on how
  to install it. We'll see this in more detail later.

Let's see how to load paletted textures.

1. Assign some VRAM memory to be used for texture palettes:

   ```c
   vramSetBankF(VRAM_F_TEX_PALETTE);
   ```

   Banks F (64 KiB), G (16 KiB) and H (16 KiB) can be used for texture palettes.

2. Generate a texture ID with `glGenTextures()` and bind with `glBindTexture()`
   as explained before.

3. Load the texture specifying the right format. How you also need to decide if
   color with index 0 is transparent or not. If you want it to be transparent,
   add `GL_TEXTURE_COLOR0_TRANSPARENT` to the texture parameters. Note that this
   setting doesn't work with `GL_RGBA` and `GL_COMPRESSED` formats.

   ```c
    if (glTexImage2D(0, 0, GL_RGB256, 64, 64, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                     statueBitmap) == 0)
    {
        printf("Failed to load texture\n");
        wait_forever();
    }
    ```

4. Load the palette specifying the format and size. This function will look for
   all VRAM banks assigned as texture palette VRAM and it will try to allocate
   the data there. It will ignore banks assigned to other types of data.

   ```c
   if (glColorTableEXT(0, 0, statuePalLen / 2, 0, 0, statuePal) == 0)
   {
       printf("Failed to load palette\n");
       wait_forever();
   }
   ```

The following example shows how to do this in practice:
[`examples/graphics_3d/paletted_textures`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/paletted_textures).
Note how the texture is loaded twice. On the left it's displayed with color 0
set as transparent. On the right it's displayed with color 0 set as opaque:

![Texture with palette](texture_with_palette.png)

## 9. Compressed textures

Compressed textures are a bit special. Instead of having two parts (texture data
and palette data) they have three: texel blocks, palette indices and palette
data.

The three parts need to be loaded to different locations in VRAM:

- Texel blocks can be stored in texture slots 0 or 2 (in libnds they are
  normally VRAM A and VRAM C).
- Palette indices are stored in texture slot 1 (VRAM B).
- Palette data is stored in palette VRAM.

Some tools (like ptexconv) generate 3 different files. Other tools only generate
two (the first two parts are concatenated and saved as a single file). This is
possible because the texel blocks data is always double the size as the palette
indices data (the palette data can have any arbitrary size, so it's always
stored separately).

`glTexImage2D()` expects the texel blocks and palette indices to be provided as
one single file. You can concatenate them manually before adding them to the
data folder of your game or you can concatenate them at runtime, the choice is
yours.

You can check the following example:
[`examples/graphics_3d/compressed_texture`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/compressed_texture).

![Compressed texture](texture_compressed.png)

The main difference is the fact that you need to use ptexconv to convert the
texture. This example comes with a shell script with the following command:

```sh
$BLOCKSDSEXT/ptexconv/ptexconv \
    -gt -ob -k FF00FF -v -f tex4x4 \
    -o data/neon \
    assets/neon.png
```

This generates a texture with magenta as transparent color of tex4x4 format. The
output files are `neon_tex.bin` (texel blocks), `neon_idx.bin` (palette indices)
and `neon_pal.bin` (palette data). Then, the script concatenates the files:

```sh
cat data/neon_tex.bin data/neon_idx.bin > data/neon_combined.bin
rm data/neon_tex.bin data/neon_idx.bin
```

The actual code that loads the texture and the palette are the same as for all
other textures with palettes:

```c
if (glTexImage2D(0, 0, GL_COMPRESSED, 128, 128, 0, TEXGEN_TEXCOORD, neon_combined_bin) == 0)
{
    printf("Failed to load texture\n");
    wait_forever();
}

if (glColorTableEXT(0, 0, neon_pal_bin_size / 2, 0, 0, neon_pal_bin) == 0)
{
    printf("Failed to load palette\n");
    wait_forever();
}
```

## 10. Final notes about textures

### 10.1 Edge texture modes

When you draw textured polygons you normally use coordinates that fit inside the
texture. For example, if your texture size is 64x64 pixels, your texture
coordinates go from 0 to 63. However, you're allowed to send values that are
outside of those bounds (negative, or bigger than 63).

You can decide the behaviour in that case when you load a texture with
`glTexImage2D()`:

- Clamp them to the texture size (default setting).
- Wrap the coordinates and repeat the texture: `GL_TEXTURE_WRAP_S` and
  `GL_TEXTURE_WRAP_T`.
- Wrap the coordinates and flip the texture. You need to use the previous flags
  and the flip flags `GL_TEXTURE_FLIP_S` and `GL_TEXTURE_FLIP_T`.

Check the following example:
[`examples/graphics_3d/texture_edge_modes`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/texture_edge_modes).

![Texture edge modes](texture_edge_modes.png)

### 10.2 Texture matrix

Apart from the projection and model view matrices you can use the texture
matrix. This texture can be used to achieve transformation effects without
having to calculate the texture coordinates by hand.

This tutorial won't get into many details, it will only show a simple example of
using it:

Check the following example:
[`examples/graphics_3d/texture_matrix`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/texture_matrix).

![Texture matrix](texture_matrix.gif)

The important things to consider are:

- The effect of the texture matrix depends on how you load textures. For
  example, in this case we use `TEXGEN_TEXCOORD` and we enable texture wrap
  because we are going to make the texture loop to create that effect.

  ```c
  if (glTexImage2D(0, 0, GL_RGBA, 256, 256, 0,
                   TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                   teapotBitmap) == 0)
  ```

- You need to setup the texture matrix. In this example we do a simple
  translation. The effect of the example appears because the texture coordinates
  of the model follow a spherical pattern. They start on the bottom of the
  teapot.

  ```c
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef32(tx << 10, ty << 10, 0);
  ```

You can check [GBATEK](https://problemkaputt.de/gbatek.htm#ds3dtextureattributes)
for all the details, but this is the short version:

- `TEXGEN_OFF`: Ignore the texture matrix.
- `TEXGEN_TEXCOORD`: Multiply texture coordinates by the texture matrix.
- `TEXGEN_NORMAL`: Set texture coordinates equal to normal * texture matrix,
  used for spherical reflection mapping.
- `TEXGEN_POSITION`: Set texture coordinates equal to vertex * texture matrix.

### 10.3 Sharing palettes between textures

If you want to use the same palette in more than one texture you can use
function `int glAssignColorTable(int target, int name)`. It will take the
palette of `name` and assign it to `target`. The palette will only be deleted
from VRAM when all the textures that use it are deleted.

### 10.4 Live editing of textures

Live editing of palettes is very useful:

- You can edit textures and palettes to create special effects, like palette
  loop effects.

- You can use them to delay loading of textures and palettes. If you pass a
  `NULL` data pointers to `glTexImage2D()` or `glColorTableEXT()` and they will
  allocate space in VRAM, but not copy anything there. You can load data later
  with this system.

There are a few functions that take a texture ID as argument and return a
pointer to the address in VRAM of the requested data:

- `glGetTexturePointer()`: Address of the texture data.
- `glGetTextureExtPointer()`: Address of the palette indices data (only
  `GL_COMPRESSED` textures).
- `glGetColorTablePointer()`: Address of the palette data.

Unfortunately, texture and palette data can't be accessed right away. You need
to switch the VRAM banks to LCD mode, edit the data, and switch back to the
previous mode. And you need to do this quickly because the GPU needs the VRAM
banks to be set to texture or texture palette mode to be able to use them. The
process to do this correctly is:

- Setup a VBL handler with your texture/palette editting code.
- In the handler, call the `glGet*Pointer()` function to get the pointer to the
  data you want to edit.
- Set the banks to LCD mode. For example, call `vramSetBankA(VRAM_A_LCD)` if
  you have assigned VRAM bank A as texture VRAM, or `vramSetBankF(VRAM_F_LCD)`
  if you have assigned VRAM bank F for palettes.
- Edit the data.
- Call `vramSetBankA(VRAM_A_TEXTURE)` or `vramSetBankF(VRAM_F_TEX_PALETTE)` as
  required.

You don't even have the full vertical blanking period to do the modifications.
The GPU starts to render lines 48 scanlines before the end of the vertical
blanking period (documentation [here](https://problemkaputt.de/gbatek.htm#ds3doverview)
and [here](https://problemkaputt.de/gbatek.htm#ds3dstatus)).

Vertical blanking starts in scanline 192, and 3D rendering starts in scanline
214. At that point, the GPU starts rendering lines and storing them in a buffer
that can hold up to 48 lines (you can check the current status by reading
`GFX_RDLINES_COUNT`). The actual screen output starts after scanline 262. In
practice, you only have 22 scanlines to upload data to VRAM. If you don't make
it in time, the GPU will read white pixels from VRAM.

## 11. Alpha blending (translucency)

There are two ways to draw polygons that aren't fully opaque:

- Setting the polygon alpha value with `glPolyFmt()` and `POLY_ALPHA()`.
- Using the translucent textures formats `GL_RGB32_A3` and `GL_RGB8_A5`.

### 11.1 Per-polygon alpha value

You can create translucency effects by setting an alpha value between 1 and 30
to polygons. Check the following example:
[`examples/graphics_3d/translucent_polygon`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/translucent_polygon).

![Translucent polygon](translucent_polygon.png)

In this example there is only one opaque polygon and one translucent polygon, so
we don't need to worry about the order in which we draw them. That will become a
problem when we start drawing multiple translucent polygons.

In this very easy example all we need to do is to enable alpha blending:

```c
glEnable(GL_BLEND);
```

And set the alpha blending for the polygons to be drawn next:

```c
glPolyFmt(POLY_ALPHA(15) | POLY_CULL_NONE);
```

{{< callout type="warning" >}}
A value of 0 doesn't make the polygon fully transparent, it enables wireframe
mode. In that mode only the outline of polygons is drawn.
{{< /callout >}}

### 11.2 Translucent textures

The previous system restricts you to one single alpha value per polygon. Using
translucent textures allows you to specify up to 8 levels of transparency with
`GL_RGB32_A3` or 32 levels with `GL_RGB8_A5`.

![Translucent texture](translucent_texture.png)

The code of the example is here:
[`examples/graphics_3d/translucent_texture`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/translucent_texture).

In this case you also need to enable alpha blending:

```c
glEnable(GL_BLEND);
```

The rest of the work involves creating a PNG file with an alpha channel. When
you convert it with grit, it will use that channel to generate the translucency
values of `GL_RGB32_A3` and `GL_RGB8_A5` textures. You need to use the following
parameters for grit:

```sh
# 5 bits of alpha, 3 bits of color index
-gx -gb -gBa5i3 -gT!
```

```sh
# 3 bits of alpha, 5 bits of color index
-gx -gb -gBa3i5 -gT!
```

### 11.3 Multiple translucent objects

So far we have only seen how to display translucent polygons on top of opaque
polygons. This is easy and you don't really need to do anything special. Opaque
polygons are drawn before translucent polygons, so they get naturally sorted by
themselves. However, if you want to draw translucent polygons on top of others
you need to manage polygon IDs.

We have ignored this so far, but `glPolyFmt()` can take a polygon ID (0 to 63)
with `POLY_ID()`. The clear plane also has its own polygon ID, which can be set
with `glClearPolyID()`. Polygon IDs are important for anti-aliasing and polygon
outlines, we will see them later.

Translucent polygons that belong to the same object (and don't blend with each
other) can have the same polygon ID. Polygons that are meant to overlap other
translucent polygons need to have different IDs.

However, the hardest part is that you need to draw all the translucent objects
in the right order (for now, ignore the part of the code that sorts the objects
by distance, we will see how to do that later). Polygons that are far away from
the camera need to be drawn first. Polygons closer to the camera need to be
drawn last. Also, you need to do this to finish the frame:

```c
glFlush(GL_TRANS_MANUALSORT);
```

If you don't pass this value, it will sort polygons by their Y coordinate, which
probably isn't what you want.

This example takes everything into consideration to draw a few translucent
balls: [`examples/graphics_3d/sort_translucent_objects`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/sort_translucent_objects).

The following image shows the regular output of this example when balls are
ordered correctly (left) and when they are sorted in reverse (right):

![Compare sort order of translucent objects](translucent_objects_sort_order.png)

In this example each ball has its own polygon ID. The following image compares
the output when all balls have different IDs (left) and when they all share the
same ID (right):

![Compare polygon ID effects on translucent objects with](translucent_objects_polygon_ids.png)

You also need to consider culling when drawing objects that are translucent.
Normally you only want to see the camera-facing polygons of the translucent
objects (left), but you can also correctly display the back-facing polygons
(right):

![Translucent objects with back and front culling](translucent_objects_cull_back_front.png)

If you want to see both sets of polygons you can't just disable culling:

![Translucent objects with no culling](translucent_objects_cull_none.png)

The problem here is that polygons are drawn in an order that isn't back to
front. You need to sort the polygons inside each object, which may not be
possible at all if you are using display lists (as we will see later).

A trick you can use is to render the objects twice:

- First, with front culling, so that the back-facing polygons are rendered.
- Second, with back culling, so that the camera-facing polygons are rendered.

Also, you need to sort the objects as well. For example, in the case of the
balls, you need to draw each ball twice as described, and then start drawing the
next ball. You can't draw all balls with front culling and then all balls with
front culling.

This is a bit complicated to see in an example with so many objects, so let's
show it in an easier example with just one cube:
[`examples/graphics_3d/translucent_cube`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/translucent_cube).

![Translucent cube](translucent_cube.png)

This is the important part of the code. You can see how we draw the back-facing
polygons first with 0 as polygon ID, and then the front-facing polygons with 1
as polygon ID:

```c
glPolyFmt(POLY_ALPHA(10) | POLY_ID(0) | POLY_CULL_FRONT);

draw_box(-0.75, -0.75, -0.75,
         0.75, 0.75, 0.75);

glPolyFmt(POLY_ALPHA(10) | POLY_ID(1) | POLY_CULL_BACK);

draw_box(-0.75, -0.75, -0.75,
         0.75, 0.75, 0.75);
```

Remember to tell the hardware that we have sorted translucent polygons manually:

```c
glFlush(GL_TRANS_MANUALSORT);
```

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
