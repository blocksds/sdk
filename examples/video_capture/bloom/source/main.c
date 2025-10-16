// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// Example based on "60 FPS Bloom Demo" by Bluescrn
// Which is based on "30 FPS Two Pass 3D Test" by eli
// Which is based on a DrunkenCoders demo by Dovoto

// This is how the process works:
//
// - Frame 0: The 3D scene is generated and captured (but not displayed on the
//   screen).
//
// - Frame 1: The 3D scene is processed and the bloom bitmap is generated. The
//   processed bitmap has a resolution of 128x96 instead of 256x192 to make it
//   easier for the CPU to finish on time.
//
// - Frame 2: The 3D scene is displayed, as well as the bloom bitmap. The bloom
//   bitmap is displayed on top of the 3D scene.
//
// Note: If you hold the A button the 3D layer will be hidden and only the bloom
// effect will be shown on the screen.

#include <nds.h>

#include "teapot_bin.h"

int main(int argc, char *argv[])
{
    glInit();

    // We don't display the 3D output directly, it needs to be processed before.
    videoSetMode(MODE_5_2D);

    // Set up the 16bit bitmap display to display the captured/processed frame
    bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    // Set up the 8bit bitmap for the half-resolution glow overlay
    bgInit(3, BgType_Bmp8, BgSize_B8_128x128, 7, 0);
    bgSetRotateScale(3, 0, (1 << 8) / 2, (1 << 8) / 2);
    bgUpdate();

    // Sort layers so that the bloom bitmap is on top of the 3D captured scene
    bgSetPriority(2, 2);
    bgSetPriority(3, 1);

    // Enable antialias
    glEnable(GL_ANTIALIAS);

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

    // Setup some material properties
    glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
    glMaterialf(GL_DIFFUSE, RGB15(24, 24, 24));
    glMaterialf(GL_SPECULAR, RGB15(0, 0, 0));
    glMaterialf(GL_EMISSION, RGB15(0, 0, 0));

    // Setup console on the sub screen
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 1, false, true);

    printf("PAD: Rotate\n");
    printf("R:   Hide 3D scene\n");
    printf("\n");
    printf("START: Exit to loader\n");

    // This palette will be used for the glow bitmap. It's a good idea to
    // generate more colors than used so that we don't need to check bounds
    // later.
    for (int i = 0; i < 256; i++)
    {
        int gi = i >> 1;
        if (gi > 31)
            gi = 31;

        int bi = i >> 0;
        if (bi > 31)
            bi = 31;

        BG_PALETTE[i] = ARGB16(1, 0, gi, bi);
    }

    const int screen_width = 256;
    const int screen_height = 192;

    const int bloom_width = screen_width / 2;
    const int bloom_height = screen_height / 2;

    const int border = 8;

    const int bloom_ext_width = bloom_width + 2 * border;
    const int bloom_ext_height = bloom_height + 2 * border;

    // Allocate 2 temporary buffers for the bloom effect. They require a 8-pixel
    // border around the image (16 pixels in total).
    u8 *tempBlurBuffers[2];
    u8 *blurBuffers[2];
    for (int n = 0; n < 2; n++)
    {
        size_t size = bloom_ext_width * bloom_ext_height;
        tempBlurBuffers[n] = calloc(1, size);

        // Set the pointer to the actual (0, 0) in the buffer....
        blurBuffers[n] = tempBlurBuffers[n] + (bloom_ext_width * border) + border;
    }

    int triple_buffer_frame = 0;

    u32 rotateX = 0;
    u32 rotateY = 0;

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

        u16 *vramSourceBmp = NULL;

        if (triple_buffer_frame == 0)
        {
            triple_buffer_frame = 1;

            vramSetBankB(VRAM_B_MAIN_BG_0x06000000); // Displaying
            vramSetBankC(VRAM_C_LCD);                // Capturing
            vramSetBankD(VRAM_D_LCD);                // Processing

            vramSourceBmp = VRAM_D;

            REG_DISPCAPCNT =
                DCAP_BANK(DCAP_BANK_VRAM_C) |   // Destination is VRAM_C
                DCAP_SIZE(DCAP_SIZE_256x192) |  // Size = 256x192
                DCAP_MODE(DCAP_MODE_A) |        // Capture source A only
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) | // Source A = 3D rendered image
                DCAP_ENABLE;                    // Enable capture
        }
        else if (triple_buffer_frame == 1)
        {
            triple_buffer_frame = 2;

            vramSetBankB(VRAM_B_LCD);                // Capturing
            vramSetBankC(VRAM_C_LCD);                // Processing
            vramSetBankD(VRAM_D_MAIN_BG_0x06000000); // Displaying

            vramSourceBmp = VRAM_C;

            REG_DISPCAPCNT =
                DCAP_BANK(DCAP_BANK_VRAM_B) |   // Destination is VRAM_B
                DCAP_SIZE(DCAP_SIZE_256x192) |  // Size = 256x192
                DCAP_MODE(DCAP_MODE_A) |        // Capture source A only
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) | // Source A = 3D rendered image
                DCAP_ENABLE;                    // Enable capture
        }
        else if (triple_buffer_frame == 2)
        {
            triple_buffer_frame = 0;

            vramSetBankB(VRAM_B_LCD);                // Processing
            vramSetBankC(VRAM_C_MAIN_BG_0x06000000); // Displaying
            vramSetBankD(VRAM_D_LCD);                // Capturing

            vramSourceBmp = VRAM_B;

            REG_DISPCAPCNT =
                DCAP_BANK(DCAP_BANK_VRAM_D) |   // Destination is VRAM_D
                DCAP_SIZE(DCAP_SIZE_256x192) |  // Size = 256x192
                DCAP_MODE(DCAP_MODE_A) |        // Capture source A only
                DCAP_SRC_A(DCAP_SRC_A_3DONLY) | // Source A = 3D rendered image
                DCAP_ENABLE;                    // Enable capture
        }

        // Only the first 6 BMP slots are used for the 256x192 screen capture.
        // We can use slot 7 as destination of the processed 128x96 bitmap.
        u16 *vramDestinationBmp = vramSourceBmp + ((7 * 16384) / sizeof(u16));

        // Draw scene

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0);

        glPushMatrix();
        {
            glRotateXi(rotateX);
            glRotateYi(rotateY);

            glCallList(teapot_bin);
        }
        glPopMatrix(1);

        glFlush(0);

        // Process frame and save it at 0x6020000 (BMP base 7)

        // Get the captured buffer from VRAM, extract the R, G and B components,
        // generate a brightness value from them, and save the result to a
        // buffer that has half width and height.
        u16 *pVram = vramSourceBmp;
        u8 *pBuffer = blurBuffers[0];

        for (int y = 0; y < screen_height; y = y + 2)
        {
            for (int x = 0; x < screen_width; x = x + 2)
            {
                u16 val = *pVram;

                s8 v = (val & 0x1F) + ((val >> 5) & 0x1F) + ((val >> 10) & 0x1F);

                // The 10 is a 'min brightness to bloom' value
                // v -= 10;
                // if (v < 0)
                //     v = 0;

                *pBuffer++ = v;
                pVram += 2;
            }

            pBuffer += 2 * border; // Skip border
            pVram += screen_width;
        }

        // Explanation of blur filter:
        //     http://www.filtermeister.com/tutorials/blur01.html

        // ks = filter box size. Max = 8 (without increasing the borders)
        int ks = 5;

        // Horizontal blur

        u8 *pSrc = blurBuffers[0];
        u8 *pDst = blurBuffers[1];

        for (int y = 0; y < bloom_height; y++)
        {
            int v = 0;

            for (int n = -ks; n <= ks; n++)
                v += pSrc[n];

            for (int x = 0; x < bloom_width; x++)
            {
                *pDst++ = v >> 3; // Adjust this shift to roughly by a divide by ks!
                v = v - pSrc[-ks];
                pSrc++;
                v = v + pSrc[ks];
            }

            pSrc += 2 * border;
            pDst += 2 * border;
        }

        // Vertical blur

        for (int x = 0; x < bloom_width; x++)
        {
            // Stride of the buffer, in bytes
            const int s = bloom_ext_width;

            int v = 0;

            pSrc = blurBuffers[1] + x;
            pDst = blurBuffers[0] + x;

            for (int n = -ks; n <= ks; n++)
                v += pSrc[s * n];

            for (int y = 0; y < bloom_height; y++)
            {
                *pDst = v >> 3; // Adjust this shift to roughly by a divide by ks!
                pDst += s;
                v = v - pSrc[-ks * s];
                pSrc += s;
                v = v + pSrc[ks * s];
            }
        }

        // Write the result to the 8 BPP layer

        // TODO: Eliminate this step, by making the v-blur process 2 or 4 pixels
        // at a time, and write them directly to the glow buffer in VRAM. :)

        u32 *pGlowVram = (u32 *)vramDestinationBmp;
        u32 *pSrcBuffer = (u32 *)blurBuffers[0];
        for (int y = 0; y < bloom_height; y++)
        {
            for (int x = 0; x < bloom_width / sizeof(u32); x++)
                *pGlowVram++ = *pSrcBuffer++;

            // Skip border
            pSrcBuffer += 2 * border / sizeof(u32);
        }

        if (keysHeld() & KEY_A)
        {
            // Turn off blending, show the top layer alone
            REG_BLDCNT = 0;
            REG_BLDALPHA = 0;
        }
        else
        {
            // Additive blend (1.0 * BG2 + 1.0 * BG3)
            REG_BLDCNT = BLEND_SRC_BG3 | BLEND_DST_BG2 | BLEND_ALPHA;
            REG_BLDALPHA = BLDALPHA_EVA(31) | BLDALPHA_EVB(31);
        }

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    for (int n = 0; n < 2; n++)
        free(tempBlurBuffers[n]);

    return 0;
}
