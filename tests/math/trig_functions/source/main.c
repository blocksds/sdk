// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to use the trigonometric routines of libnds.

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&bottomScreen);

    printf("sinLerp(-45) = %f\n", f32tofloat(sinLerp(degreesToAngle(-45))));
    printf("sinLerp(-30) = %f\n", f32tofloat(sinLerp(degreesToAngle(-30))));
    printf("sinLerp(0) = %f\n", f32tofloat(sinLerp(degreesToAngle(0))));
    printf("sinLerp(30) = %f\n", f32tofloat(sinLerp(degreesToAngle(30))));
    printf("sinLerp(45) = %f\n", f32tofloat(sinLerp(degreesToAngle(45))));
    printf("sinLerp(60) = %f\n", f32tofloat(sinLerp(degreesToAngle(60))));
    printf("sinLerp(90) = %f\n", f32tofloat(sinLerp(degreesToAngle(90))));
    printf("sinLerp(135) = %f\n", f32tofloat(sinLerp(degreesToAngle(135))));
    printf("sinLerp(180) = %f\n", f32tofloat(sinLerp(degreesToAngle(180))));
    printf("sinLerp(270) = %f\n", f32tofloat(sinLerp(degreesToAngle(270))));
    printf("\n");
    printf("cosLerp(-45) = %f\n", f32tofloat(cosLerp(degreesToAngle(-45))));
    printf("cosLerp(-30) = %f\n", f32tofloat(cosLerp(degreesToAngle(-30))));
    printf("cosLerp(0) = %f\n", f32tofloat(cosLerp(degreesToAngle(0))));
    printf("cosLerp(30) = %f\n", f32tofloat(cosLerp(degreesToAngle(30))));
    printf("cosLerp(45) = %f\n", f32tofloat(cosLerp(degreesToAngle(45))));
    printf("cosLerp(60) = %f\n", f32tofloat(cosLerp(degreesToAngle(60))));
    printf("cosLerp(90) = %f\n", f32tofloat(cosLerp(degreesToAngle(90))));
    printf("cosLerp(135) = %f\n", f32tofloat(cosLerp(degreesToAngle(135))));
    printf("cosLerp(180) = %f\n", f32tofloat(cosLerp(degreesToAngle(180))));
    printf("cosLerp(270) = %f\n", f32tofloat(cosLerp(degreesToAngle(270))));
    printf("\n");
    printf("Press A to continue");

    consoleSelect(&topScreen);

    printf("tanLerp(-90) = %f\n", f32tofloat(tanLerp(degreesToAngle(-90))));
    printf("tanLerp(-60) = %f\n", f32tofloat(tanLerp(degreesToAngle(-60))));
    printf("tanLerp(-45) = %f\n", f32tofloat(tanLerp(degreesToAngle(-45))));
    printf("tanLerp(-30) = %f\n", f32tofloat(tanLerp(degreesToAngle(-30))));
    printf("tanLerp(0) = %f\n", f32tofloat(tanLerp(degreesToAngle(0))));
    printf("tanLerp(30) = %f\n", f32tofloat(tanLerp(degreesToAngle(30))));
    printf("tanLerp(45) = %f\n", f32tofloat(tanLerp(degreesToAngle(45))));
    printf("tanLerp(60) = %f\n", f32tofloat(tanLerp(degreesToAngle(60))));
    printf("tanLerp(90) = %f\n", f32tofloat(tanLerp(degreesToAngle(90))));
    printf("tanLerp(120) = %f\n", f32tofloat(tanLerp(degreesToAngle(120))));
    printf("tanLerp(135) = %f\n", f32tofloat(tanLerp(degreesToAngle(135))));
    printf("tanLerp(150) = %f\n", f32tofloat(tanLerp(degreesToAngle(150))));
    printf("tanLerp(180) = %f\n", f32tofloat(tanLerp(degreesToAngle(180))));
    printf("tanLerp(225) = %f\n", f32tofloat(tanLerp(degreesToAngle(225))));
    printf("tanLerp(270) = %f\n", f32tofloat(tanLerp(degreesToAngle(270))));

    while (1)
    {
        scanKeys();
        if (keysDown() & KEY_A)
            break;
        swiWaitForVBlank();
    }

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("asinLerp(-1.0) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)-1.0)));
    printf("asinLerp(-0.707) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)-0.707)));
    printf("asinLerp(-0.5) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)-0.5)));
    printf("asinLerp(0.0) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)0.0)));
    printf("asinLerp(0.5) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)0.5)));
    printf("asinLerp(0.707) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)0.707)));
    printf("asinLerp(1.0) = %d\n", angleToDegrees(asinLerp((1 << 12) * (float)1.0)));
    printf("\n");
    printf("acosLerp(-1.0) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)-1.0)));
    printf("acosLerp(-0.707) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)-0.707)));
    printf("acosLerp(-0.5) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)-0.5)));
    printf("acosLerp(0.0) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)0.0)));
    printf("acosLerp(0.5) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)0.5)));
    printf("acosLerp(0.707) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)0.707)));
    printf("acosLerp(1.0) = %d\n", angleToDegrees(acosLerp((1 << 12) * (float)1.0)));
    printf("\n");
    printf("Press A to continue");

    consoleSelect(&topScreen);
    consoleClear();

    printf("atanLerp(tan(-90)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(-90)))));
    printf("atanLerp(tan(-80)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(-80)))));
    printf("atanLerp(tan(-60)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(-60)))));
    printf("atanLerp(tan(-45)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(-45)))));
    printf("atanLerp(tan(-30)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(-30)))));
    printf("atanLerp(tan(0)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(0)))));
    printf("atanLerp(tan(10)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(10)))));
    printf("atanLerp(tan(20)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(20)))));
    printf("atanLerp(tan(30)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(30)))));
    printf("atanLerp(tan(45)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(45)))));
    printf("atanLerp(tan(60)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(60)))));
    printf("atanLerp(tan(70)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(70)))));
    printf("atanLerp(tan(80)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(80)))));
    printf("atanLerp(tan(85)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(85)))));
    printf("atanLerp(tan(87)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(87)))));
    printf("atanLerp(tan(88)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(88)))));
    printf("atanLerp(tan(89)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(89)))));
    printf("atanLerp(tan(90)) = %d\n", angleToDegrees(atanLerp(tanLerp(degreesToAngle(90)))));

    while (1)
    {
        scanKeys();
        if (keysDown() & KEY_A)
            break;
        swiWaitForVBlank();
    }

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("Press START to exit to loader\n");

    consoleSelect(&topScreen);
    consoleClear();

#define radToDegrees(x) (int)((x) * 360 / (2 * 3.1415))

    printf("atan2_f32(1, 0) = %d\n", radToDegrees(f32tofloat(atan2_f32(1, 0))));
    printf("atan2_f32(0, 1) = %d\n", radToDegrees(f32tofloat(atan2_f32(0, 1))));
    printf("atan2_f32(-1, 0) = %d\n", radToDegrees(f32tofloat(atan2_f32(-1, 0))));
    printf("atan2_f32(0, -1) = %d\n", radToDegrees(f32tofloat(atan2_f32(0, -1))));
    printf("atan2_f32(1, 1) = %d\n", radToDegrees(f32tofloat(atan2_f32(1, 1))));
    printf("atan2_f32(1, -1) = %d\n", radToDegrees(f32tofloat(atan2_f32(1, -1))));
    printf("atan2_f32(-1, -1) = %d\n", radToDegrees(f32tofloat(atan2_f32(-1, -1))));
    printf("atan2_f32(-1, 1) = %d\n", radToDegrees(f32tofloat(atan2_f32(-1, 1))));

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
