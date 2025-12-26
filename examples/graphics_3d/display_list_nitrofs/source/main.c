// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This example shows how to convert a model in Wavefront OBJ format to a
// display list (check the `convert.sh` script) and display it on the DS with
// glCallList().

#include <stdio.h>

#include <nds.h>
#include <filesystem.h>

__attribute__((noreturn)) void wait_forever(void)
{
    printf("Press START to exit.");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            exit(1);
    }
}

bool file_load(const char *path, void **buffer, size_t *size)
{
    // Open the file in read binary mode
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        perror("fopen");
        return false;
    }

    // Move read cursor to the end of the file
    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return false;
    }

    // Check position of the cursor (we're at the end, so this is the size)
    size_t size_ = ftell(f);
    if (size_ == 0)
    {
        printf("Size is 0!");
        fclose(f);
        return false;
    }

    // Move cursor to the start of the file again
    rewind(f);

    // Allocate buffer to hold data
    *buffer = malloc(size_);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        return false;
    }

    // Read all data into the buffer
    if (fread(*buffer, size_, 1, f) != 1)
    {
        perror("fread");
        fclose(f);
        free(*buffer);
        return false;
    }

    // Close file
    ret = fclose(f);
    if (ret != 0)
    {
        perror("fclose");
        free(*buffer);
        return false;
    }

    if (size)
        *size = size_;

    return true;
}

int main(int argc, char *argv[])
{
    consoleDemoInit();

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit() failed\n");
        wait_forever();
    }

    void *teapot_bin;
    if (!file_load("teapot.bin", &teapot_bin, NULL))
    {
        printf("Failed to load file\n");
        wait_forever();
    }

    videoSetMode(MODE_0_3D);

    glInit();

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

    // Setup lights
    glLight(0, RGB15(0, 31, 0), floattov10(-1), floattov10(0), floattov10(0));
    glLight(1, RGB15(0, 31, 0), floattov10(0), floattov10(0), floattov10(-1));

    int angle_x = 45;
    int angle_z = 45;

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

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK |
                  POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);

        glCallList(teapot_bin);

        glFlush(0);
    }

    free(teapot_bin);

    return 0;
}
