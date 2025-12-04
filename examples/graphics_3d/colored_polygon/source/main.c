// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    // Setup sub screen for the text console
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_ANTIALIAS);

    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // antialias works.
    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    int angle_x = 0;
    int angle_z = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        consoleClear();

        // Print some controls
        printf("PAD:    Rotate quad\n");
        printf("START:  Exit to loader\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_LEFT)
            angle_z += 3;
        if (keys & KEY_RIGHT)
            angle_z -= 3;

        if (keys & KEY_UP)
            angle_x += 3;
        if (keys & KEY_DOWN)
            angle_x -= 3;

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        glRotateZ(angle_z);
        glRotateX(angle_x);

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

        glPopMatrix(1);

        glFlush(0);
    }

    return 0;
}
