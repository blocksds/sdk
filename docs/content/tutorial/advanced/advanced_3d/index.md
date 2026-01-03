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

{{< callout type="error" >}}
This chapter is a work in progress...
{{< /callout >}}
