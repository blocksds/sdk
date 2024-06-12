// Demonstrates how to use 3D picking on the DS
//
// Author: Gabe Ghearing
// Created: Feb 2007
//
// This file is released into the public domain
//
// Basic idea behind picking:
//
//     Draw the scene a second time with a projection matrix that only renders
// what is directly below the cursor. The GPU keeps track of how many polygons
// are drawn, so if a polygon is drawn in this limited view the polygon is
// directly below the cursor.  Several polygons may be drawn under the cursor,
// so a position test is used for each object(a collection of polygons) to tell
// which object is closest to the camera.  The object that is closest to the
// camera and under the cursor is the one that the user is clicking on.
//
// There are several optimizations that are not done in this example, such as:
//
// - Simplify models during the picking pass, the model needs to occupy the same
//   area, but can usually use fewer polygons.
//
// - Save the projection matrix with glGetFixed() instead of recreating it every
//   pass.

#include <nds.h>
#include <nds/arm9/postest.h>
#include "cone_bin.h"
#include "cylinder_bin.h"
#include "sphere_bin.h"

typedef enum {
    NOTHING,
    CONE,
    CYLINDER,
    SPHERE
} Clickable;

Clickable clicked;  // What is being clicked
int closeW;         // Closest distace to camera
int polyCount;      // Keeps track of the number of polygons drawn

// Run before starting to draw an object while picking
void startCheck(void)
{
    // Wait for the position test to finish
    while (PosTestBusy());

    // Wait for all the polygons from the last object to be drawn
    while (GFX_BUSY);

    // Start a position test at the current translated position
    PosTest_Asynch(0, 0, 0);

    // Save the polygon count
    polyCount = GFX_POLYGON_RAM_USAGE;
}

// Run afer drawing an object while picking
void endCheck(Clickable obj)
{
    // Wait for all the polygons to get drawn
    while (GFX_BUSY);

    // Wait for the position test to finish
    while (PosTestBusy());

    if (GFX_POLYGON_RAM_USAGE > polyCount) // If a polygon was drawn
    {
        if (PosTestWresult() <= closeW)
        {
            // This is currently the closest object under the cursor!
            closeW = PosTestWresult();
            clicked = obj;
        }
    }
}

int main(int argc, char *argv[])
{
    // Initialize GL
    glInit();

    u32 rotateX = 0;
    u32 rotateY = 0;

    // Set mode 0, enable BG0 and set it to 3D
    videoSetMode(MODE_0_3D);

    // We are going to be touching the 3D display, place the 3D scene on the
    // touchscreen.
    lcdMainOnBottom();

    // Enable edge outlining, this will be used to show which object is selected
    glEnable(GL_OUTLINE);

    // Set the first outline color to white
    glSetOutlineColor(0, RGB15(31, 31, 31));

    // Used later for gluPickMatrix()
    int viewport[] = { 0, 0, 255, 191 };

    // Setup the rear plane
    glClearColor(0, 0, 0, 0); // Set BG to black
    // The BG and polygons will have the same ID unless a polygon is highlighted
    glClearPolyID(0);
    glClearDepth(0x7FFF);

    // Setup the camera
    gluLookAt(0.0, 0.0, 1.0,  // Camera possition
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    // Setup the light
    glLight(0, RGB15(31, 31, 31), 0, floattov10(-1.0), 0);

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

        // Get touchscreen position
        touchPosition touchXY;
        touchRead(&touchXY);

        // Set the viewport to fullscreen
        glViewport(0, 0, 255, 191);

        // Setup the projection matrix for regular drawing
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 256.0 / 192.0, 0.1, 20);

        // Use the modelview matrix while drawing
        glMatrixMode(GL_MODELVIEW);

        // Save the state of the current matrix(the modelview matrix)
        glPushMatrix();
        {
            glTranslatef32(0, 0, floattof32(-6));
            glRotateXi(rotateX); // Add X rotation to the modelview matrix
            glRotateYi(rotateY); // Add Y rotation to the modelview matrix

            // Save the state of the modelview matrix while making the first pass
            glPushMatrix();
            {
                // Draw the scene for displaying

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(2.9), floattof32(0), floattof32(0));
                if (clicked == CONE)
                {
                    // Set a poly ID for outlining
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(1));
                }
                else
                {
                    // Set a poly ID for no outlining (same as BG)
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(0));
                }
                // Draw a green cone from a predefined packed command list
                glCallList(cone_bin);

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(-3), floattof32(1.8), floattof32(2));
                if (clicked == CYLINDER)
                {
                    // Set a poly ID for outlining
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(1));
                }
                else
                {
                    // Set a poly ID for no outlining (same as BG)
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(0));
                }
                // Draw a blue cylinder from a predefined packed command list
                glCallList(cylinder_bin);

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(.5), floattof32(-2.6), floattof32(-4));
                if (clicked == SPHERE)
                {
                    // Set a poly ID for outlining
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(1));
                }
                else
                {
                    // Set a poly ID for no outlining (same as BG)
                    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0 | POLY_ID(0));
                }
                // Draw a red sphere from a predefined packed command list
                glCallList(sphere_bin);
            }
            // Restores the modelview matrix to where it was just rotated
            glPopMatrix(1);

            // Draw the scene again for picking
            {
                clicked = NOTHING; //reset what was clicked on
                closeW = 0x7FFFFFFF; //reset the distance

                // Set the viewport to just off-screen, this hides all rendering
                // that will be done during picking.
                glViewport(0, 192, 0, 192);

                // Setup the projection matrix for picking
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                // Render only what is below the cursor
                gluPickMatrix(touchXY.px, 191 - touchXY.py, 4, 4, viewport);
                // This must be the same as the original perspective matrix
                gluPerspective(60, 256.0 / 192.0, 0.1, 20);

                // Switch back to modifying the modelview matrix for drawing
                glMatrixMode(GL_MODELVIEW);

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(2.9), floattof32(0), floattof32(0));
                startCheck();
                // Draw a cone from a predefined packed command list
                glCallList(cone_bin);
                endCheck(CONE);

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(-3), floattof32(1.8), floattof32(2));
                startCheck();
                // Draw a cylinder from a predefined packed command list
                glCallList(cylinder_bin);
                endCheck(CYLINDER);

                // Translate the modelview matrix to the drawing location
                glTranslatef32(floattof32(.5), floattof32(-2.6), floattof32(-4));
                startCheck();
                // Draw a sphere from a predefined packed command list
                glCallList(sphere_bin);
                endCheck(SPHERE);
            }

        }
        // Restores the modelview matrix to its original state
        glPopMatrix(1);

        // Wait for everything to be drawn before starting on the next frame
        glFlush(0);

        swiWaitForVBlank();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
