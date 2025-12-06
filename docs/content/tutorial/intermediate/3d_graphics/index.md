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

## 2. Initialization

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
}
```

A downside of using the 3D engine is that you need to tell the GPU what to draw
every frame. With the 2D hardware you don't have to do anything once you've
configured your background and sprites unless something changes and, when
something changes, all you need to do is reconfigure the specific thing that has
changed. With the 3D hardware you need to send all the polygons to the GPU every
frame, which takes some CPU time. At the end of this chapter we will learn how
to use display lists, which is one of the ways to optimize 3D rendering.

## 3. Rendering a polygon

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

## 4. Moving the camera

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

## 5. Culling

When you draw polygons you send the vertices to the GPU in a specific order.
This order is used by the GPU to decide whether the polygon is facing the camera
or away from it. If vertices are counter-clockwise when rendered on the screen
the GPU considers the polygon to be facing the camera.

The exampl [`examples/graphics_3d/basic_cube`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/basic_cube)
shows the effects of using back culling (on the left) or front culling (on the
right). Back culling shows the model as expected because it doesn't draw the
polygons that you don't normally see. Front culling hides the polygons you
normally see:

![Culling](culling.png)

Normally you won't be defining polygons by hand, so you don't have to worry
about the order of the polygons. Your 3D modelling program will export vertices
in the right format.

## 6. Work in progress

...
