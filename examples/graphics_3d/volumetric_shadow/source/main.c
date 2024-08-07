// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

#include "teapot_bin.h"
#include "teapot.h"

void DrawFloor(void)
{
    glNormal3f(0, -0.97, 0);

    glBegin(GL_QUAD);

        glTexCoord2t16(0, 0);
        glVertex3f(-10, 0, -10);

        glTexCoord2t16(0, inttot16(256));
        glVertex3f(-10, 0, 10);

        glTexCoord2t16(inttot16(256), inttot16(256));
        glVertex3f(10, 0, 10);

        glTexCoord2t16(inttot16(256), 0);
        glVertex3f(10, 0, -10);

    glEnd();
}

void DrawLid(void)
{
    glNormal3f(0, -0.97, 0);

    glBegin(GL_QUAD);

        glTexCoord2t16(0, 0);
        glVertex3f(-0.75, 3, -0.75);

        glTexCoord2t16(0, inttot16(256));
        glVertex3f(-0.75, 3,  0.75);

        glTexCoord2t16(inttot16(256), inttot16(256));
        glVertex3f( 0.75, 3,  0.75);

        glTexCoord2t16(inttot16(256), 0);
        glVertex3f( 0.75, 3, -0.75);

    glEnd();
}

void DrawShadowVolume(void)
{
    // The bottom of the volume has to be slightly lower than the floor of the
    // room because of lack of accuracy.

    // Lid

    glBegin(GL_QUAD);

        glVertex3f(-0.75, 3, -0.75);
        glVertex3f(-0.75, 3,  0.75);
        glVertex3f( 0.75, 3,  0.75);
        glVertex3f( 0.75, 3, -0.75);

    glEnd();

    // Walls

    glBegin(GL_QUAD_STRIP);

        glVertex3f(-0.75, 3, -0.75);
        glVertex3f(-0.75, -0.5, -0.75);
        glVertex3f(-0.75, 3,  0.75);
        glVertex3f(-0.75, -0.5,  0.75);

        glVertex3f( 0.75, 3,  0.75);
        glVertex3f( 0.75, -0.5,  0.75);

        glVertex3f( 0.75, 3, -0.75);
        glVertex3f( 0.75, -0.5, -0.75);

        glVertex3f(-0.75, 3, -0.75);
        glVertex3f(-0.75, -0.5, -0.75);

    glEnd();

    // Bottom

    glBegin(GL_QUAD);

        glVertex3f(-0.75, -0.5, -0.75);
        glVertex3f(-0.75, -0.5,  0.75);
        glVertex3f( 0.75, -0.5,  0.75);
        glVertex3f( 0.75, -0.5, -0.75);

    glEnd();
}

void DrawTeapot(void)
{
    glCallList(teapot_bin);
}

int main(int argc, char *argv[])
{
    glInit();
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);

    // Setup the rear plane
    glClearColor(0, 0, 0, 0); // Set BG to black
    // The BG and polygons will have the same ID unless a polygon is highlighted
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImage2D(0, 0, GL_RGBA, 256, 256, 0,
                     TEXGEN_TEXCOORD | GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T,
                     teapotBitmap) == 0)
    {
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMaterialShinyness();

    // Set material properties
    glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
    glMaterialf(GL_DIFFUSE, RGB15(24, 24, 24));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 20);

    printf("ABXY:    Rotate\n"
           "Pad:     Move\n"
           "SELECT:  Show edges of shadow\n"
           "START:   Exit to loader\n");

    // Set initial position of the object
    int32_t x = floattof32(0);
    int32_t y = floattof32(1.5);
    int32_t z = floattof32(0);

    int32_t rx = 0;
    int32_t ry = 0;
    int32_t rz = 0;

    while (1)
    {
        swiWaitForVBlank();

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        // Move model using the pad
        if (keys & KEY_UP)
            z += floattof32(0.05);
        if (keys & KEY_DOWN)
            z -= floattof32(0.05);
        if (keys & KEY_RIGHT)
            x -= floattof32(0.05);
        if (keys & KEY_LEFT)
            x += floattof32(0.05);

        // Rotate model using the pad
        if (keys & KEY_Y)
            rz += 2;
        if (keys & KEY_B)
            rz -= 2;
        if (keys & KEY_X)
            ry += 2;
        if (keys & KEY_A)
            ry -= 2;

        bool draw_edges = false;
        if (keys & KEY_SELECT)
            draw_edges = true;

        if (keys & KEY_START)
            break;

        // Use the modelview matrix while drawing
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        // Setup the camera
        gluLookAt(0, 3.25, -3.25,
                  0, 1.25, 0,
                  0, 1, 0);

        glLight(0, RGB15(31, 31, 31), 0, floattov10(-0.97), 0);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_MODULATION);

        glBindTexture(0, textureID);

        // Draw regular models

        glPushMatrix();
            glTranslate3f32(x, y, z);
            glRotateXi(rx << 6);
            glRotateYi(ry << 6);
            glRotateZi(rz << 6);
            DrawTeapot();
        glPopMatrix(1);

        DrawFloor();
        DrawLid();

        // Draw shadow volume as a black volume (shadow)

        glBindTexture(0, 0);

        glColor3f(0, 0, 0);

        if (draw_edges)
        {
            // Draw the shadow volume in wireframe mode to see where it is
            glPolyFmt(POLY_ALPHA(0) | POLY_ID(0) |  POLY_CULL_NONE | POLY_MODULATION);
            DrawShadowVolume();
        }

        glPolyFmt(POLY_ALPHA(1) | POLY_ID(0) |  POLY_CULL_NONE | POLY_SHADOW);
        DrawShadowVolume();

        glPolyFmt(POLY_ALPHA(20) | POLY_ID(63) |  POLY_CULL_NONE | POLY_SHADOW);
        DrawShadowVolume();

        glFlush(GL_TRANS_MANUALSORT);
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
