---
title: Advanced 3D techniques
weight: 2
---

## 1. Introduction

This chapter is a continuation of the [3D graphics](../../intermediate/3d_graphics)
chapter that focuses on more advanced techniques.

## 2. W vs Z buffering

The DS allows you to pick between Z and W depth buffering. All you need to do is
to call `glFlush(GL_ZBUFFERING)` (the default if you don't specify anything) or
`glFlush(GL_WBUFFERING)` (not recommended for orthogonal projections).

Z-buffering distributes depth values uniformly between the near plane and the
far plane, so it works better when your polygons are distributed uniformly
between them. W-buffering is more accurate for polygons close to the near plane,
and less accurate for polygons close to the far plane. This may make it more
useful for regular 3D scenes.

The GPU uses fixed point for all its calculations, and it's easy to see
[Z-fighting](https://en.wikipedia.org/wiki/Z-fighting) between polygons that
don't have enough space between them. Having a bit more of accuracy can help
with this kind of issues. This image shows the same scene with Z-buffering on
the left and W-buffering on the right, and it proves how W-buffering is more
accurate in this specific case:

![Depth buffering modes](depth_buffering_modes.png)

You can test the example yourself here:
[`examples/graphics_3d/depth_buffering_modes`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/depth_buffering_modes)

## 2. Billboards

Billboards are 2D objects that are displayed in a 3D scene, but always facing
the camera.

![Billboards](billboards.png)

This can be done for different reasons:

- You want to display a 2D status bar on top of a character or enemy.
- You want to draw shapes that are hard to draw in 3D, like a sphere, but you
  don't want to use many polygons to do it. In this case, a 2D quad with a
  texture that displays a circle could be convincing enough for your players.

Displaying polygons that always face the camera requires you to know the
direction the camera is facing right now. We can do that by getting the vector
matrix. This is a 3x3 matrix that is transformed the same way as the position
and projection matrix, but with unit vectors.

From that matrix we can extract the three vectors that face up, right and front
from the perspective of the camera, and create a matrix that is the inverse of
it. The vector matrix has unit vectors, so we can invert it by transposing it:

```c
int matrix[9];
glGetFixed(GL_GET_MATRIX_VECTOR, matrix);

int32_t right[3] = { matrix[0], matrix[3], matrix[6] };
int32_t up[3] =    { matrix[1], matrix[4], matrix[7] };
int32_t front[3] = { matrix[2], matrix[5], matrix[8] };

// Create the inverse matrix of the vector matrix
const m3x3 to_billboard_space = {{
     right[0],  right[1],  right[2],
        up[0],     up[1],     up[2],
    -front[0], -front[1], -front[2],
}};
```

When we multiply the current modelview matrix by `to_billboard_space` we will
switch from using the 3D world axes to using axes aligned to the screen. For
example, if you're drawing a quad that represents a ball:

```c
glPushMatrix();

    // Move to the position of the ball first
    glTranslate3f32(ball.x, ball.y, ball.z);

    // Rotate the world so that the axes are in billboard space
    glMultMatrix3x3(&to_billboard_space);

    // Now we can scale the billboard if we want
    glScalef32(inttof32(ball.scale), inttof32(ball.scale), inttof32(1));

    glBegin(GL_QUADS);

        glTexCoord2t16(inttot16(32), 0);
        glVertex3v16(floattov16(-0.5), floattov16(1), 0);

        glTexCoord2t16(0, 0);
        glVertex3v16(floattov16(0.5), floattov16(1), 0);

        glTexCoord2t16(0, inttot16(32));
        glVertex3v16(floattov16(0.5), 0, 0);

        glTexCoord2t16(inttot16(32), inttot16(32));
        glVertex3v16(floattov16(-0.5), 0, 0);

    glEnd();

glPopMatrix(1);
```

You can check the example here:
[`examples/graphics_3d/billboards`](https://github.com/blocksds/sdk/tree/master/examples/graphics_3d/billboards)

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
