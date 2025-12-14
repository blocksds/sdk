// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This example shows how to convert a model in Wavefront OBJ format to a
// display list (check the `convert.sh` script) and display it on the DS with
// glCallList().
//
// It also lets you enable wireframe mode so that you can see the polygons that
// form the model, and you can change the culling settings to experiment.

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
    gluLookAt(0.0, 2.0, 6.0,  // Camera position
              0.0, 1.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 20);

    // Use the modelview matrix while drawing
    glMatrixMode(GL_MODELVIEW);

    // Setup some material properties
    glMaterialf(GL_AMBIENT, RGB15(0, 0, 0));
    glMaterialf(GL_DIFFUSE, RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glLight(0, RGB15(0, 31, 0), floattov10(-1), floattov10(0), floattov10(0));
    glLight(1, RGB15(0, 31, 0), floattov10(0), floattov10(0), floattov10(-1));

    int angle_x = 45;
    int angle_z = 45;

    bool wireframe = false;
    int culling = POLY_CULL_NONE;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        consoleClear();

        // Print some controls
        printf("PAD:     Rotate\n");
        printf("\n");
        printf("A:       Enable eireframe\n");
        printf("B:       Disable eireframe\n");
        printf("\n");
        printf("L:       Cull front\n");
        printf("R:       Cull back\n");
        printf("SELECT:  Cull none\n");
        printf("\n");
        printf("START:   Exit to loader\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_UP)
            angle_x += 3;
        if (keys & KEY_DOWN)
            angle_x -= 3;

        if (keys & KEY_LEFT)
            angle_z += 3;
        if (keys & KEY_RIGHT)
            angle_z -= 3;

        if (keys & KEY_L)
            culling = POLY_CULL_FRONT;
        if (keys & KEY_R)
            culling = POLY_CULL_BACK;
        if (keys & KEY_SELECT)
            culling = POLY_CULL_NONE;

        if (keys & KEY_A)
            wireframe = true;
        if (keys & KEY_B)
            wireframe = false;

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        // Setup camera
        glLoadIdentity();
        gluLookAt(0.0, 0.0, 4.0,  // Position
                  0.0, 0.0, 0.0,  // Look at
                  0.0, 1.0, 0.0); // Up

        glRotateY(angle_z);
        glRotateX(angle_x);

        glPolyFmt(POLY_ALPHA(wireframe ? 0 : 31) | culling |
                  POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);

        glCallList(teapot_bin);

        glFlush(0);
    }

    return 0;
}
