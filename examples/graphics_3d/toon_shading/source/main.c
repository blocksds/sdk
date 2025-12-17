// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

#include "teapot_bin.h"

int main(int argc, char *argv[])
{
    glInit();
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    // Setup the rear plane
    glClearColor(0, 0, 0, 0); // Set BG to black
    // The BG and polygons will have the same ID unless a polygon is highlighted
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    // Setup the camera
    gluLookAt(0.0, 0.0, 4.0,  // Camera position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Setup the light
    glLight(0, RGB15(31, 31, 31),
            floattov10(0.58), floattov10(-0.58), floattov10(-0.58));

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 20);

    // Use the modelview matrix while drawing
    glMatrixMode(GL_MODELVIEW);

    // Define two ranges in the table. The first range is for pixels with less
    // than half the maximum light intensity, and the second one is for pixels
    // that have over half the maximum light intensity. This way the regular
    // smooth shading is reduced to two shades.
    glSetToonTableRange(0, 15, RGB15(8, 8, 8));
    glSetToonTableRange(16, 31, RGB15(28, 28, 28));

    // Setup some material properties
    glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
    glMaterialf(GL_DIFFUSE, RGB15(24, 24, 24));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Create table of values for specular materials (unused in this example,
    // but needed in general).
    glMaterialShinyness();

    u32 rotateX = 0;
    u32 rotateY = 0;

    printf("PAD: Rotate\n");
    printf("A: Disable toon shading\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        // Handle key input
        scanKeys();
        u16 keys = keysHeld();
        if (keys & KEY_UP)
            rotateX += 3 << 5;
        if (keys & KEY_DOWN)
            rotateX -= 3 << 5;
        if (keys & KEY_LEFT)
            rotateY += 3 << 5;
        if (keys & KEY_RIGHT)
            rotateY -= 3 << 5;

        if (keys & KEY_A)
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 |
                      POLY_ID(0) | POLY_DECAL);
        else
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 |
                      POLY_ID(0) | POLY_TOON_HIGHLIGHT);

        glPushMatrix();
        {
            glRotateXi(rotateX);
            glRotateYi(rotateY);

            glCallList(teapot_bin);
        }
        glPopMatrix(1);

        glFlush(0);

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
