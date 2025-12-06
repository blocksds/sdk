// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

const uint32_t screen_half_width = screen_width / 2;
const uint32_t screen_half_height = screen_height / 2;

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

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

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
        printf("START:   Exit to loader\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70, 256.0 / 192.0, 0.1, 40);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0, 0.0, 3.0,  // Position
                  0.0, 0.0, 0.0,  // Look at
                  0.0, 1.0, 0.0); // Up

        glRotateY(angle_z);
        glRotateX(angle_x);

        angle_x ++;
        angle_z ++;

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));

        draw_box(-1.0, -1.0, -1.0,
                 1.0, 1.0, 1.0);

        // Render 2D primitives
        // --------------------

        // Set up GL2D for 2D mode
        glBegin2D();

            // Top left quadrant of the screen

            glBoxFilledGradient(0, 0,
                                screen_half_width - 1, screen_half_height - 1,
                                RGB15(31, 0, 0),
                                RGB15(31, 31, 0),
                                RGB15(31, 0, 31),
                                RGB15(0, 31, 31));

            // Bottom left quadrant of the screen

            glPolyFmt(POLY_ALPHA(16) | POLY_CULL_NONE | POLY_ID(1));

            glTriangleFilledGradient(0, screen_half_height,
                                     screen_half_width - 1, screen_height * 3 / 4,
                                     0, screen_height - 1,
                                     RGB15(0, 31, 0),
                                     RGB15(31, 0, 0),
                                     RGB15(0, 0, 31));

        glEnd2D();

        glFlush(0);
    }

    return 0;
}
