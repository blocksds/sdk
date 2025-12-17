// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

// This example shows the effect of different material properties on the same
// object:
//
// - Ambient: It's added to the object if there are lights enabled, but it
//   illuminates it uniformly. For example, if there is an object in a corner of
//   a room, which isn't hit by the light, but the object needs to be more
//   illuminated if the room is more illuminated, and dark if the room is dark.
// - Diffuse: It's affected by the direction of the light in a linear way. This
//   is what you normally want to use for mate objects.
// - Specular: It works in a similar way as diffuse, but it has a more metallic
//   aspect. You can define the way this behaves by setting the shinyness table
//   manually.
// - Emission: This is the minimum color that the object will have, even if
//   there are no external sources of light. This can be used for a lamp, for
//   example.

#include <nds.h>

#include "teapot_bin.h"

typedef struct {
    u16 ambient, diffuse, specular, emission;
    bool libnds_shinyness_table;
    bool custom_shinyness_table;
    char *line1;
    char *line2;
    char *line3;
} material_info_t;

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
    gluLookAt(0.0, 0.0, 4.0,  // Camera position
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

#define MAX_MATERIAL_SETUPS 8

    const material_info_t materials[MAX_MATERIAL_SETUPS] = {
        {
            RGB15(8, 8, 8), RGB15(24, 24, 24), RGB15(0, 0, 0), RGB15(0, 0, 0),
            false, false,
            "Mate object with ambient light",
            "Like in a room with light",
            NULL
        },
        {
            RGB15(0, 0, 0), RGB15(31, 31, 31), RGB15(0, 0, 0), RGB15(0, 0, 0),
            false, false,
            "Mate object, no ambient light",
            "Like in a dark room.",
            NULL
        },
        {
            RGB15(24, 24, 24), RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(0, 0, 0),
            false, false,
            "Ambient light only. It uses",
            "active light colors as uniform",
            "color for all vertices."
        },
        {
            RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(20, 20, 20),
            false, false,
            "Emission light only, unaffected",
            "by external light.",
            "Try to disable the light"
        },
        {
            RGB15(0, 0, 0), RGB15(16, 16, 16), RGB15(0, 0, 0), RGB15(20, 20, 20),
            false, false,
            "Emission and ambient light",
            "affected by external light.",
            "Try to disable the light"
        },
        {
            RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(31, 31, 31), RGB15(0, 0, 0),
            false, false,
            "Specular material.",
            "Like metal.",
            "Shininess table: None",
        },
        {
            RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(31, 31, 31), RGB15(0, 0, 0),
            true, false,
            "Specular material",
            "Like metal.",
            "Shininess table: libnds",
        },
        {
            RGB15(0, 0, 0), RGB15(0, 0, 0), RGB15(31, 31, 31), RGB15(0, 0, 0),
            false, true,
            "Specular material",
            "Like metal.",
            "Shininess table: custom",
        }
    };

    bool light_enabled[2] = { true, true };
    int material_group = 0;

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

        u16 keys_down = keysDown();
        if (keys_down & KEY_L)
        {
            material_group--;
            if (material_group == -1)
                material_group = MAX_MATERIAL_SETUPS - 1;
        }
        else if (keys_down & KEY_R)
        {
            material_group++;
            if (material_group == MAX_MATERIAL_SETUPS)
                material_group = 0;
        }

        if (keys_down & KEY_A)
            light_enabled[0] = true;
        else if (keys_down & KEY_B)
            light_enabled[0] = false;

        if (keys_down & KEY_X)
            light_enabled[1] = true;
        else if (keys_down & KEY_Y)
            light_enabled[1] = false;

        if (light_enabled[0])
        {
            glLight(0, RGB15(0, 31, 0),
                    floattov10(0.58), floattov10(-0.58), floattov10(-0.58));
        }
        else
        {
            glLight(0, 0, 0, 0, 0);
        }

        if (light_enabled[1])
        {
            glLight(1, RGB15(0, 0, 31),
                    floattov10(-0.58), floattov10(-0.58), floattov10(-0.58));
        }
        else
        {
            glLight(1, 0, 0, 0, 0);
        }

        consoleClear();

        printf("START: Exit to loader\n");
        printf("\n");
        printf("PAD: Rotate\n");
        printf("A/B: Light 0: %s\n", light_enabled[0] ? "On" : "Off");
        printf("X/Y: Light 1: %s\n", light_enabled[1] ? "On" : "Off");
        printf("L/R: Change materials\n");
        printf("\n");
        printf("\n");

        const material_info_t *material = &materials[material_group];

        printf("Material %d\n\n", material_group);

        printf("%s\n%s\n%s\n\n",
               material->line1 ?  material->line1 : "",
               material->line2 ?  material->line2 : "",
               material->line3 ?  material->line3 : "");

        consoleSetCursor(NULL, 0, 15);

        printf("  Ambient:  (%d, %d, %d)\n", material->ambient & 31,
               (material->ambient >> 5) & 31, (material->ambient >> 10) & 31);
        printf("  Diffuse:  (%d, %d, %d)\n", material->diffuse & 31,
               (material->diffuse >> 5) & 31, (material->diffuse >> 10) & 31);
        printf("  Specular: (%d, %d, %d)\n", material->specular & 31,
               (material->specular >> 5) & 31, (material->specular >> 10) & 31);
        printf("  Emission: (%d, %d, %d)\n", material->emission & 31,
               (material->emission >> 5) & 31, (material->emission >> 10) & 31);

        printf("\n");

        if (material->libnds_shinyness_table)
            printf("  Shininess table: libnds\n");
        else if (material->custom_shinyness_table)
            printf("  Shininess table: custom\n");
        else
            printf("  Shininess table: none\n");

        // Setup some material properties
        glMaterialf(GL_AMBIENT, material->ambient);
        glMaterialf(GL_DIFFUSE, material->diffuse);
        if (material->libnds_shinyness_table || material->custom_shinyness_table)
            glMaterialf(GL_SPECULAR, material->specular | GL_SPECULAR_USE_TABLE);
        else
            glMaterialf(GL_SPECULAR, material->specular); // Linear table
        glMaterialf(GL_EMISSION, material->emission);

        if (material->libnds_shinyness_table)
        {
            // Create table of values for specular materials
            glMaterialShininess();
        }
        else if (material->custom_shinyness_table)
        {
            uint32_t table[128 / 4];
            uint8_t *bytes = (uint8_t *)table;
#if 1
            // Cubic function
            for (int i = 0; i < 128; i++)
            {
                int v = i * i * i;
                int div = 128 * 128;
                bytes[i] = v * 2 / div;
            }
#else
            // Quartic function
            for (int i = 0; i < 128; i++)
            {
                int v = i * i * i * i;
                int div = 128 * 128 * 128;
                bytes[i] = v * 2 / div;
            }
#endif
            for (int i = 0; i < 128 / 4; i++)
                GFX_SHININESS = table[i];
        }
        else
        {
            // Do nothing
        }

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 |
                  POLY_FORMAT_LIGHT1);

        glPushMatrix();
        {
            glRotateXi(rotateX);
            glRotateYi(rotateY);

            glCallList(teapot_bin);
        }
        glPopMatrix(1);

        glFlush(0);

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
