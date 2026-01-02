// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    // Wait for a couple of frames to give time to the ARM7 to read the settings
    // with readUserSettings().
    swiWaitForVBlank();
    swiWaitForVBlank();

    ssize_t len;

    consoleDemoInit();

    printf("User settings:\n\n");

    char name[50];
    len = utf16_to_utf8(name, sizeof(name), (char16_t *)PersonalData->name,
                        PersonalData->nameLen * sizeof(char16_t));
    if (len < 0)
        strcpy(name, "#Decode error#");
    printf("  Name: %s\n", name);

    const char *month[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    printf("  Birthday: %d %s\n", PersonalData->birthDay,
            month[PersonalData->birthMonth - 1]);

    const char *lang[8] = {
        "Japanese", "English", "French", "German", "Italian", "Spanish",
        "Chinese(?)", "Unknown"
    };
    printf("  Language: %s\n", lang[PersonalData->language]);

    printf("\n");
    char message[50];
    len = utf16_to_utf8(message, sizeof(message), (char16_t *)PersonalData->message,
                        PersonalData->messageLen * sizeof(char16_t));
    if (len < 0)
        strcpy(message, "#Decode error#");
    printf("  Message:\n\n%s\n", message);
    printf("\n");

    printf("  Alarm time: %u:%02u\n", PersonalData->alarmHour, PersonalData->alarmMinute);
    printf("  GBA screen: %s\n", PersonalData->gbaScreen ? "Lower" : "Upper");
    printf("  Autoboot: %s\n", PersonalData->autoMode ? "On" : "Off");

    const char *color[16] = {
        "Gray", "Brown", "Red", "Pink",
        "Orange", "Yellow", "Yellow/Green-ish", "Green",
        "Dark Green", "Green/Blue-ish", "Light Blue", "Blue",
        "Dark Blue", "Dark Purple", "Purple", "Purple/Red-ish",
    };
    printf("  Theme: %s\n", color[PersonalData->theme]);

    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
