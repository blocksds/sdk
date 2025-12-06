// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

// This example shows how to draw a simple 3D box and the effect of the culling
// setting.

#include <stdio.h>

#include <nds.h>

void draw_box(float bx_, float by_, float bz_, float ex_, float ey_, float ez_)
{
    // Begin and end coordinates
    int bx = floattov16(bx_);
    int ex = floattov16(ex_);
    int by = floattov16(by_);
    int ey = floattov16(ey_);
    int bz = floattov16(bz_);
    int ez = floattov16(ez_);

    glBegin(GL_QUADS);

        glColor3f(1, 0, 0);

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, ey, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, by, bz);

        glColor3f(0, 1, 0);

        glVertex3v16(bx, by, ez);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(bx, ey, ez);

        glColor3f(0, 0, 1);

        glVertex3v16(bx, by, bz);
        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, by, ez);
        glVertex3v16(bx, by, ez);

        glColor3f(1, 0, 1);

        glVertex3v16(bx, ey, bz);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

        glColor3f(0, 1, 1);

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(bx, ey, bz);

        glColor3f(1, 1, 0);

        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, by, ez);

    glEnd();
}

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

    // Setup perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    // Switch to model view matrix
    glMatrixMode(GL_MODELVIEW);

    int angle_x = 45;
    int angle_z = 45;

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    int culling = POLY_CULL_NONE;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        consoleClear();

        // Print some controls
        printf("PAD:     Move\n");
        printf("A,B,X,Y: Rotate\n");
        printf("L:       Cull front\n");
        printf("R:       Cull back\n");
        printf("SELECT:  Cull none\n");
        printf("\n");
        printf("START:   Exit to loader\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_LEFT)
            x -= 0.05;
        if (keys & KEY_RIGHT)
            x += 0.05;

        if (keys & KEY_UP)
            y += 0.05;
        if (keys & KEY_DOWN)
            y -= 0.05;

        if (keys & KEY_A)
            angle_x += 3;
        if (keys & KEY_Y)
            angle_x -= 3;

        if (keys & KEY_X)
            angle_z += 3;
        if (keys & KEY_B)
            angle_z -= 3;

        if (keys & KEY_L)
            culling = POLY_CULL_FRONT;
        if (keys & KEY_R)
            culling = POLY_CULL_BACK;
        if (keys & KEY_SELECT)
            culling = POLY_CULL_NONE;

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        // Setup camera
        glLoadIdentity();
        gluLookAt(0.0, 0.0, 4.0,  // Position
                  0.0, 0.0, 0.0,  // Look at
                  0.0, 1.0, 0.0); // Up

        // Move and rotate the view before drawing a box
        glTranslatef(x, y, z);

        glRotateY(angle_z);
        glRotateX(angle_x);

        glPolyFmt(POLY_ALPHA(31) | culling);

        draw_box(-0.75, -0.75, -0.75,
                 0.75, 0.75, 0.75);

        glFlush(0);
    }

    return 0;
}
