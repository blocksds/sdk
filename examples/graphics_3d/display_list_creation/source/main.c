// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

// A display list is just a list of commands sent to the GPU as one block of
// data. It is made of 32-bit words:
//
// The first word of the display list is the size (in words) of the display
// list (excluding the size itself!).
//
// Next, there is a word which is a block of 4 packed GPU commands (like
// glBegin(), glColor(), glVertex()...). After each word with commands you need
// to add words with the argument that each command needs. If a command needs
// more than one argument, it must be split into multiple words (like
// FIFO_VERTEX16, which needs to be split into two).
//
// The last word with commands can be padded with FIFO_NOP commands, which
// doesn't take any arguments.
const uint32_t display_list[] =
{
    13,

    FIFO_COMMAND_PACK(FIFO_BEGIN, FIFO_COLOR, FIFO_VERTEX16, FIFO_COLOR),
        GL_QUADS,
        RGB15(31, 0, 0),
        VERTEX_PACK(inttov16(-1), inttov16(-1)),
        VERTEX_PACK(0, 0),
        RGB15(0, 31, 0),

    FIFO_COMMAND_PACK(FIFO_VERTEX_XY, FIFO_COLOR, FIFO_VERTEX_XY, FIFO_COLOR),
        VERTEX_PACK(inttov16(1), inttov16(-1)),
        RGB15(0, 0, 31),
        VERTEX_PACK(inttov16(1), inttov16(1)),
        RGB15(31, 31, 0),

    FIFO_COMMAND_PACK(FIFO_VERTEX_XY, FIFO_END, FIFO_NOP, FIFO_NOP),
        VERTEX_PACK(inttov16(-1), inttov16(1)),
};

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
    gluLookAt(0.0, 0.0, 4.0,  // Camera possition
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Set the viewport to fullscreen
    glViewport(0, 0, 255, 191);

    // Setup the projection matrix for regular drawing
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 20);

    // Use the modelview matrix while drawing
    glMatrixMode(GL_MODELVIEW);

    u32 rotateX = 0;
    u32 rotateY = 0;

    printf("PAD: Rotate\n");
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

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0) | POLY_DECAL);

        glPushMatrix();
        {
            glRotateXi(rotateX);
            glRotateYi(rotateY);

            glCallList(display_list);
        }
        glPopMatrix(1);

        glFlush(0);

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
