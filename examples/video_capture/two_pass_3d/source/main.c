// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

// Two-pass 3D example. This example uses the video capture hardware to increase
// the number of polygons that can be drawn on one screen. It shows one way of
// doing it. It draws the left and right halves of the screen in different
// passes. It requires special perspective matrices on both halves, but that's
// the only unusual thing to take care of. Both halves have full Z-buffer
// support, so it isn't needed to sort polygons before sending them to the GPU.
//
// Other methods of doing two-pass 3D are to draw the far polygons first, and
// then the near polygons on top. The problem is that the Z-buffer is lost
// between the two passes.
//
// Note that in both cases the framerate will be reduced to 30 FPS.

#include <stdio.h>

#include <nds.h>

// This is created automatically from neon.png and neon.grit
#include "neon.h"

// Align this to 4 bytes because we will use DMA to copy words from this buffer.
u16 framebuffer[2][256 * 192] ALIGN(4);

// Index of the framebuffer currently displayed.
volatile u8 framebuffer_displayed = 0;

// The main loop sets this to true when the next frame is available to be
// displayed.
volatile bool framebuffer_next_ready = false;

void DrawTexturedCube(void)
{
    int bx = floattov16(-0.5);
    int ex = floattov16(0.5);
    int by = floattov16(-0.5);
    int ey = floattov16(0.5);
    int bz = floattov16(-0.5);
    int ez = floattov16(0.5);

    glBegin(GL_QUADS);

        glNormal(NORMAL_PACK(0, 0, floattov10(-0.97)));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, bz);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, bz);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, 0, floattov10(0.97)));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, ez);

        glNormal(NORMAL_PACK(0, floattov10(-0.97), 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, by, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, by, bz);

        glNormal(NORMAL_PACK(0, floattov10(0.97), 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, ey, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

        glNormal(NORMAL_PACK(floattov10(-0.97), 0, 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(bx, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(bx, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(bx, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(bx, ey, bz);

        glNormal(NORMAL_PACK(floattov10(0.97), 0, 0));

        glTexCoord2t16(0, 0);
        glVertex3v16(ex, by, bz);
        glTexCoord2t16(0, inttot16(128));
        glVertex3v16(ex, by, ez);
        glTexCoord2t16(inttot16(128), inttot16(128));
        glVertex3v16(ex, ey, ez);
        glTexCoord2t16(inttot16(128), 0);
        glVertex3v16(ex, ey, bz);

    glEnd();
}

void DrawGouraudCube(void)
{
    int bx = floattov16(-0.5);
    int ex = floattov16(0.5);
    int by = floattov16(-0.5);
    int ey = floattov16(0.5);
    int bz = floattov16(-0.5);
    int ez = floattov16(0.5);

    glBegin(GL_QUADS);

        glColor(RGB15(31, 0, 0));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, ey, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, by, bz);

        glColor(RGB15(0, 31, 0));

        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, by, ez);

        glColor(RGB15(0, 0, 31));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, by, bz);

        glColor(RGB15(31, 0, 31));

        glVertex3v16(bx, ey, bz);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

        glColor(RGB15(31, 31, 0));

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(bx, ey, bz);

        glColor(RGB15(0, 31, 31));

        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

    glEnd();
}

int textureID;
float rx = 30, ry = 0;
float rotateX = 45.0;
float rotateY = 45.0;

void DrawScene(void)
{
    gluLookAt(0.0, 0.0, 1.0,    // Camera position
              0.0, 0.0, 0.0,    // Look at
              0.0, 1.0, 0.0);   // Up

    glPushMatrix();
    {
        // Move cube left and away from the camera
        glTranslatef32(floattof32(-0.5), 0, floattof32(-1));

        glRotateX(rx);
        glRotateY(ry);

        glBindTexture(0, textureID);

        DrawTexturedCube();
    }
    glPopMatrix(1);

    glPushMatrix();
    {
        // Move cube right and away from the camera
        glTranslatef32(floattof32(0.5), 0, floattof32(-1));

        glRotateX(rotateX);
        glRotateY(rotateY);

        // Deactivate texture
        glBindTexture(0, 0);

        DrawGouraudCube();
    }
    glPopMatrix(1);
}

void vbl_handler(void)
{
    dmaStopSafe(2);

    // Switch the rendered buffer only when the main loop has finished drawing
    // the next frame.
    if (framebuffer_next_ready)
    {
        framebuffer_next_ready = false;
        framebuffer_displayed ^= 1;
    }

    void *fb = framebuffer[framebuffer_displayed];

    // The source address must be in main RAM.
    REG_DMA_SRC(2) = (uintptr_t)fb;
    // The destination must be REG_DISP_MMEM_FIFO.
    REG_DMA_DEST(2) = (uintptr_t)&REG_DISP_MMEM_FIFO;

    REG_DMA_CR(2) =
        DMA_DISP_FIFO | // Set direct FIFO display mode
        DMA_SRC_INC |   // Increment the source address each time
        DMA_DST_FIX |   // Fix the destination address to REG_DISP_MMEM_FIFO
        DMA_REPEAT |    // Don't stop after the first transfer
        DMA_COPY_WORDS | 4; // Copy 4 words each time (8 pixels)
}

int main(int argc, char *argv[])
{
    powerOn(POWER_ALL);

    // Reset the VRAM banks setup so that we can load the console to VRAM H
    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    videoSetModeSub(MODE_0_2D);

    // Initialize console
    vramSetBankH(VRAM_H_SUB_BG);
    consoleInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x256, 4, 0, false, true);

    // Setup main engine to display from main RAM
    videoSetMode(MODE_FIFO);

    // Use VRAM D as destination of our video captures
    vramSetBankD(VRAM_D_LCD);

    glInit();

    glEnable(GL_TEXTURE_2D | GL_ANTIALIAS);

    // Setup the rear plane. It must have a unique polygon ID for antialias to
    // work.
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    vramSetBankA(VRAM_A_TEXTURE);

    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    if (glTexImageNtr2D(GL_RGBA, 128, 128, TEXGEN_TEXCOORD, neonBitmap, NULL) == 0)
    {
        consoleDemoInit();
        printf("Failed to load texture\n");
        while (1)
            swiWaitForVBlank();
    }

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMaterialf(GL_AMBIENT,  RGB15(5, 5, 5));
    glMaterialf(GL_DIFFUSE,  RGB15(31, 31, 31));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0);

    glLight(0, RGB15(31, 31, 31),
            floattov10(-0.6), floattov10(-0.6), floattov10(-0.6));

    irqSet(IRQ_VBLANK, vbl_handler);

    printf("START: Exit to loader\n");

    const int horizontal_split = 128;
    int frame = 0;

    while (1)
    {
        // The video syncronization has to be done before any work is done in a
        // frame. This ensures that emulators and hardware have time to
        // synchronize during the first frame. If not, screens may be switched.
        swiWaitForVBlank();

        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_START)
            break;

        // If the user presses A, wait in a loop until the button is released.
        // This is a test to see that a framerate drop won't cause any issues.
        if (keys & KEY_A)
        {
            while (1)
            {
                swiWaitForVBlank();
                scanKeys();
                if (!(keysHeld() & KEY_A))
                    break;
            }
        }

        REG_DISPCAPCNT =
            // Destination is VRAM_D
            DCAP_BANK(DCAP_BANK_VRAM_D) |
            // Size = 256x192
            DCAP_SIZE(DCAP_SIZE_256x192) |
            // Capture source A only
            DCAP_MODE(DCAP_MODE_A) |
            // Source A = 3D rendered image
            DCAP_SRC_A(DCAP_SRC_A_3DONLY) |
            // Enable capture
            DCAP_ENABLE;

        if (frame == 0)
        {
            const int split = horizontal_split;

            // Copy the last left side projection

            const u16 *fb_src = VRAM_D;
            u16 *fb_dst = framebuffer[framebuffer_displayed ^ 1];

            for (int j = 0; j < 192; j++)
            {
                dmaCopy(fb_src, fb_dst, split * sizeof(u16));
                fb_src += 256;
                fb_dst += 256;
            }

            // Setup left side projection

            glViewport(0, 0, split - 1, 191);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glFrustumf32(-128 * 3, (-128 + split) * 3, 96 * 3, -96 * 3,
                         floattof32(0.1), floattof32(40));
        }
        else
        {
            const int split = horizontal_split;

            // Copy the last right side projection

            const u16 *fb_src = VRAM_D + split;
            u16 *fb_dst = framebuffer[framebuffer_displayed ^ 1] + split;

            for (int j = 0; j < 192; j++)
            {
                dmaCopy(fb_src, fb_dst, (256 - split) * sizeof(u16));
                fb_src += 256;
                fb_dst += 256;
            }

            // Setup right side projection

            glViewport(split, 0, 255, 191);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glFrustumf32((-128 + split) * 3, (-128 + 256) * 3, 96 * 3, -96 * 3,
                         floattof32(0.1), floattof32(40));

            framebuffer_next_ready = true;
        }

        // Draw the 3D scene as usual regardless of which side we're drawing

        glClearColor(7, 7, 20, 31);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        DrawScene();

        glFlush(0);

        // Only update the scene every other frame

        if (frame == 1)
        {
            rx += 2;
            ry += 2;

            rotateX += 2;
            rotateY -= 2;
        }

        frame ^= 1;
    }

    glDeleteTextures(1, &textureID);

    return 0;
}
