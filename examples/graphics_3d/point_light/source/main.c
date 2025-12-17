// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows how to fake spot lights using the hardware directional
// lights of the GPU. Essentially, for each object and each light, we need to
// calculate the direction from the light to the object, calculate the distance
// to it, and use the distance to calculate the intensity of the light. The
// direction of the light is the direction calculated below after turning the
// vector into an unitary vector.

#include <nds.h>

#include "teapot_bin.h"

void draw_box(int32_t cx, int32_t cy, int32_t cz, // Center
              int32_t sx, int32_t sy, int32_t sz, // Size
              int r, int g, int b)                // Color
{
    // Begin and end coordinates
    int bx = f32tov16(cx) - (f32tov16(sx) / 2);
    int ex = f32tov16(cx) + (f32tov16(sx) / 2);
    int by = f32tov16(cy) - (f32tov16(sy) / 2);
    int ey = f32tov16(cy) + (f32tov16(sy) / 2);
    int bz = f32tov16(cz) - (f32tov16(sz) / 2);
    int ez = f32tov16(cz) + (f32tov16(sz) / 2);

    glColor(RGB15(r, g, b));

    glBegin(GL_QUADS);

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, ey, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, by, bz);

        glVertex3v16(bx, by, ez);
        glVertex3v16(ex, by, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(bx, ey, ez);

        glVertex3v16(bx, by, bz);
        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, by, ez);
        glVertex3v16(bx, by, ez);

        glVertex3v16(bx, ey, bz);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, ey, bz);

        glVertex3v16(bx, by, bz);
        glVertex3v16(bx, by, ez);
        glVertex3v16(bx, ey, ez);
        glVertex3v16(bx, ey, bz);

        glVertex3v16(ex, by, bz);
        glVertex3v16(ex, ey, bz);
        glVertex3v16(ex, ey, ez);
        glVertex3v16(ex, by, ez);

    glEnd();
}

void draw_3_axis(int32_t x, int32_t y, int32_t z, // Coordinates
                 int r, int g, int b)             // Color
{
    glColor(RGB15(r, g, b));

    glPushMatrix();

    // This will let us go over the limit of +/-8.0 of v16
    glScalef32(x, y, z);

    glBegin(GL_TRIANGLES);

        glVertex3v16(0, 0, 0);
        glVertex3v16(inttov16(1), 0, 0);
        glVertex3v16(inttov16(1), 0, 0);

        glVertex3v16(inttov16(1), 0,           0);
        glVertex3v16(inttov16(1), 0, inttov16(1));
        glVertex3v16(inttov16(1), 0, inttov16(1));

        glVertex3v16(inttov16(1),           0, inttov16(1));
        glVertex3v16(inttov16(1), inttov16(1), inttov16(1));
        glVertex3v16(inttov16(1), inttov16(1), inttov16(1));

    glEnd();

    glPopMatrix(1);
}

// Subtract vectors: r[] = a[] - b[]
static inline void vec_sub(int32_t *a, int32_t *b, int32_t *r)
{
    r[0] = a[0] - b[0];
    r[1] = a[1] - b[1];
    r[2] = a[2] - b[2];
}

// Calculate length of a vector
static inline int32_t vec_length(int32_t *v)
{
    return sqrtf32(mulf32(v[0], v[0]) + mulf32(v[1], v[1]) + mulf32(v[2], v[2]));
}

// Divide a vector by a constant: r[] = a[] / b
static inline void vec_div(int32_t *a, int32_t b, int32_t *r)
{
    r[0] = divf32(a[0], b);
    r[1] = divf32(a[1], b);
    r[2] = divf32(a[2], b);
}

// It setups hardware light "light_id" to point from "light_pos" to "obj_pos".
// It will use an attenuation factor of "attenuation", and the color of the
// light will be set to (r, g, b). This needs to be called for every light for
// every object.
void setup_point_light(int light_id, int32_t *light_pos, int32_t *obj_pos,
                       int32_t attenuation, int r, int g, int b)
{
    int32_t light_to_obj[3];
    vec_sub(obj_pos, light_pos, light_to_obj);

    int32_t distance = vec_length(light_to_obj);

    int32_t light_to_obj_unit[3];
    vec_div(light_to_obj, distance, light_to_obj_unit);

    int32_t distance_square = mulf32(distance, distance);

    int32_t r_calc = divf32(inttof32(r), mulf32(attenuation, distance_square));
    int32_t g_calc = divf32(inttof32(g), mulf32(attenuation, distance_square));
    int32_t b_calc = divf32(inttof32(b), mulf32(attenuation, distance_square));

    if (r_calc > r)
        r_calc = r;
    if (g_calc > g)
        g_calc = g;
    if (b_calc > b)
        b_calc = b;

    // Setup the light
    glLight(light_id, RGB15(r_calc, g_calc, b_calc),
            floattov10(f32tofloat(light_to_obj_unit[0])),
            floattov10(f32tofloat(light_to_obj_unit[1])),
            floattov10(f32tofloat(light_to_obj_unit[2])));
}

typedef struct {
    int32_t pos[3];
    int r, g, b;
    int32_t attenuation;
} light_info;

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

    // Create table of values for specular materials (unused in this example,
    // but needed in general).
    glMaterialShinyness();

    int32_t teapot_pos[2][3] = {
        { floattof32(1.9), floattof32(1.5), floattof32(0) },
        { floattof32(-1.6), floattof32(1.7), floattof32(1.5) },
    };

    light_info light[2] = {
        {
            { floattof32(2.9), floattof32(1.4), floattof32(1.1) },
            31, 15, 0, floattof32(1000.0)
        },
        {
            { floattof32(-0.4), floattof32(1.4), floattof32(2.0) },
            10, 0, 31, floattof32(100.0)
        },
    };

    printf("PAD/X/B: Move object\n");
    printf("\n");
    printf("PAD/X/B:          Light 1\n");
    printf("PAD/X/B + SELECT: Light 0\n");
    printf("PAD/X/B + L:      Teapot 0\n");
    printf("PAD/X/B + R:      Teapot 1\n");
    printf("\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        // Handle key input
        scanKeys();
        u16 keys = keysHeld();

        if (keys & KEY_L)
        {
            if (keys & KEY_UP)
                teapot_pos[0][1] += 3 << 5;
            if (keys & KEY_DOWN)
                teapot_pos[0][1] -= 3 << 5;
            if (keys & KEY_LEFT)
                teapot_pos[0][0] -= 3 << 5;
            if (keys & KEY_RIGHT)
                teapot_pos[0][0] += 3 << 5;
            if (keys & KEY_X)
                teapot_pos[0][2] -= 3 << 5;
            if (keys & KEY_B)
                teapot_pos[0][2] += 3 << 5;
        }
        else if (keys & KEY_R)
        {
            if (keys & KEY_UP)
                teapot_pos[1][1] += 3 << 5;
            if (keys & KEY_DOWN)
                teapot_pos[1][1] -= 3 << 5;
            if (keys & KEY_LEFT)
                teapot_pos[1][0] -= 3 << 5;
            if (keys & KEY_RIGHT)
                teapot_pos[1][0] += 3 << 5;
            if (keys & KEY_X)
                teapot_pos[1][2] -= 3 << 5;
            if (keys & KEY_B)
                teapot_pos[1][2] += 3 << 5;
        }
        else if (keys & KEY_SELECT)
        {
            if (keys & KEY_UP)
                light[0].pos[1] += 3 << 5;
            if (keys & KEY_DOWN)
                light[0].pos[1] -= 3 << 5;
            if (keys & KEY_LEFT)
                light[0].pos[0] -= 3 << 5;
            if (keys & KEY_RIGHT)
                light[0].pos[0] += 3 << 5;
            if (keys & KEY_X)
                light[0].pos[2] -= 3 << 5;
            if (keys & KEY_B)
                light[0].pos[2] += 3 << 5;
        }
        else
        {
            if (keys & KEY_UP)
                light[1].pos[1] += 3 << 5;
            if (keys & KEY_DOWN)
                light[1].pos[1] -= 3 << 5;
            if (keys & KEY_LEFT)
                light[1].pos[0] -= 3 << 5;
            if (keys & KEY_RIGHT)
                light[1].pos[0] += 3 << 5;
            if (keys & KEY_X)
                light[1].pos[2] -= 3 << 5;
            if (keys & KEY_B)
                light[1].pos[2] += 3 << 5;
        }

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);

        glPushMatrix();
        {
            setup_point_light(0, light[0].pos, teapot_pos[0],
                              light[0].attenuation, light[0].r, light[0].g, light[0].b);
            setup_point_light(1, light[1].pos, teapot_pos[0],
                              light[1].attenuation, light[1].r, light[1].g, light[1].b);

            glTranslatef32(teapot_pos[0][0], teapot_pos[0][1], teapot_pos[0][2]);

            glCallList(teapot_bin);
        }
        glPopMatrix(1);

        glPushMatrix();
        {
            setup_point_light(0, light[0].pos, teapot_pos[1],
                              light[0].attenuation, light[0].r, light[0].g, light[0].b);
            setup_point_light(1, light[1].pos, teapot_pos[1],
                              light[1].attenuation, light[1].r, light[1].g, light[1].b);

            glTranslatef32(teapot_pos[1][0], teapot_pos[1][1], teapot_pos[1][2]);

            glCallList(teapot_bin);
        }
        glPopMatrix(1);

        // Debug information on the 3D screen
        // ----------------------------------

        // This draws some polygons so that it's easier to identify the position
        // of each object and light on the screen.

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        // Draw directions to each teapod
        draw_3_axis(teapot_pos[0][0], teapot_pos[0][1], teapot_pos[0][2],
                    20, 20, 20);
        draw_3_axis(teapot_pos[1][0], teapot_pos[1][1], teapot_pos[1][2],
                    20, 20, 20);

        // Draw directions to each light, and the light boxes
        draw_3_axis(light[0].pos[0], light[0].pos[1], light[0].pos[2],
                    light[0].r, light[0].g, light[0].b);
        draw_box(light[0].pos[0], light[0].pos[1], light[0].pos[2],
                 floattof32(0.1), floattof32(0.1), floattof32(0.1),
                 light[0].r, light[0].g, light[0].b);

        draw_3_axis(light[1].pos[0], light[1].pos[1], light[1].pos[2],
                    light[1].r, light[1].g, light[1].b);
        draw_box(light[1].pos[0], light[1].pos[1], light[1].pos[2],
                 floattof32(0.1), floattof32(0.1), floattof32(0.1),
                 light[1].r, light[1].g, light[1].b);

        // Draw box at origin of coordinates
        draw_box(0, 0, 0,
                 floattof32(0.1), floattof32(0.1), floattof32(0.1),
                 20, 20, 20);

        glFlush(0);

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
