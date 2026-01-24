// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

#include <stdio.h>

#include <nds.h>

void draw_box(float bx_, float by_, float bz_, float sx_, float sy_, float sz_)
{
    // Begin and end coordinates
    int bx = floattov16(bx_);
    int ex = floattov16(bx_ + sx_);
    int by = floattov16(by_);
    int ey = floattov16(by_ + sy_);
    int bz = floattov16(bz_);
    int ez = floattov16(bz_ + sz_);

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
    // alpha blending works.
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

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    int vertices = 0;
    int polygons = 0;

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
        printf("START:   Exit to loader\n");
        printf("\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_LEFT)
            x -= 0.1;
        if (keys & KEY_RIGHT)
            x += 0.1;

        if (keys & KEY_UP)
            y += 0.1;
        if (keys & KEY_DOWN)
            y -= 0.1;

        if (keys & KEY_A)
            angle_x += 3;
        if (keys & KEY_Y)
            angle_x -= 3;

        if (keys & KEY_X)
            angle_z += 3;
        if (keys & KEY_B)
            angle_z -= 3;

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        glTranslatef(x, y, z);

        glRotateY(angle_z);
        glRotateX(angle_x);

        // Requisites mentioned in GBATEK:
        //
        //     https://problemkaputt.de/gbatek.htm#ds3dtests
        //
        // Render polygons that intersect with far plane and 1-dot polygons
        // behind GFX_CUTOFF_DEPTH. This is required for the box test.
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_RENDER_FAR_POLYS |
                  POLY_RENDER_1DOT_POLYS);
        // Update the polygon formats by using glBegin()
        glBegin(GL_TRIANGLES);
        glEnd();

        int in = BoxTestf(-0.5, -0.5, -0.5, // Position
                          1.0, 1.0, 1.0);   // Size

        printf("Is box inside viewport? %s\n", in ? "Yes" : "No");
        printf("\n");
        printf("Vertex count:  %4d\n", vertices);
        printf("Polygon count: %4d\n", polygons);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        // Once we know if the box is inside the screen viewport we can decide
        // to draw the polygons or not. This can save CPU time because we don't
        // spend CPU time sending polygons to the GPU that will be discarded
        // right away.
        //
        // In this example we always draw it so that we can double-check that
        // the value of the variable matches the display.
        //if (in)
        //{
            draw_box(-0.5, -0.5, -0.5, // Position
                     1.0, 1.0, 1.0);   // Size
        //}

        glPopMatrix(1);

        glGetInt(GL_GET_VERTEX_RAM_COUNT, &vertices);
        glGetInt(GL_GET_POLYGON_RAM_COUNT, &polygons);

        glFlush(0);
    }

    return 0;
}
