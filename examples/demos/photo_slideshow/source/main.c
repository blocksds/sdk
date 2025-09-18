// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Edoardo Lolletti, 2025

#include <nds.h>
#include <fat.h>
#include <filesystem.h>
#include <stdio.h>
#include <dirent.h>

#include "tjpgd.h"

#define SCROLL_INCREMENT 10

#define SOURCE_WIDTH 640
#define SOURCE_HEIGHT 480

#define BG_WIDTH (SOURCE_WIDTH / 2)
#define BG_HEIGHT (SOURCE_HEIGHT / 2)

static TWL_BSS char pool[0x8000] __attribute__((aligned(4)));
uint16_t *backbuffer = NULL;
DIR *dcim_dirp = NULL;
int bg;

const char* DCIM = "nand2:/photo/DCIM";

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

static size_t infunc(JDEC* state, uint8_t* out, size_t len)
{
    if(out == NULL)
        return fseek((FILE*)state->device, len, SEEK_CUR) == 0 ? len : 0;
    return fread(out, 1, len, (FILE*)state->device);
}

static int outfunc(JDEC* state, void* indata, JRECT* rect)
{
    vu16* in = (vu16*)indata;
    for (int j = rect->top; j <= rect->bottom; j++)
    {
        for (int i = rect->left; i <= rect->right; i++)
        {
            backbuffer[512 * j + i] = *in++;
        }
    }
    return 1;
}

// pictures are stored in the path:
// nand2:/photo/DCIM/100NINXX/HNI_XXXX.JPG
// iterate both the subfolders to find all the jpg files
DIR *get_next_dcim_subfolder(bool* rewund)
{
    chdir(DCIM);
    bool has_rewund = false;
    while(1)
    {
        struct dirent *cur_dcim = readdir(dcim_dirp);
        if (cur_dcim == NULL) {
            if(has_rewund)
                return NULL;
            has_rewund = true;
            rewinddir(dcim_dirp);
            continue;
        }

        if(cur_dcim->d_type != DT_DIR)
            continue;

        if (*cur_dcim->d_name == 0)
            continue;

        DIR *dirp = opendir(cur_dcim->d_name);
        if(dirp != NULL)
        {
            chdir(cur_dcim->d_name);
            *rewund = has_rewund;
            return dirp;
        }
    }
}

const char* get_next_picture()
{
    static DIR *dirp = NULL;
    bool was_null = dirp == NULL;
    bool has_rewund = false;
    while (1)
    {
        if(dirp == NULL)
        {
            if(has_rewund && was_null)
                return NULL;
            dirp = get_next_dcim_subfolder(&has_rewund);
            if(!dirp)
                return NULL;
        }
        struct dirent *cur = readdir(dirp);
        if (cur == NULL)
        {
            closedir(dirp);
            dirp = NULL;
            continue;
        }

        if(cur->d_type == DT_DIR)
            continue;

        if (*cur->d_name == 0)
            continue;

        return cur->d_name;
    }
}

bool showImage(const char* file)
{
    FILE* f = fopen(file, "rb");

    if(!f)
    {
        printf("Failed to open file: %s\n", file);
        return false;
    }

    JDEC jpg_state;
    JRESULT res = jd_prepare(&jpg_state, infunc, pool, sizeof(pool), f);

    // The ds takes photos with a resolution of 640x480, if the decoded image
    // doesn't have these dimensions, we bail out since it's a nonstandard scenario
    if(jpg_state.width != SOURCE_WIDTH || jpg_state.height != SOURCE_HEIGHT)
    {
        printf("Image %s has mismatching dimensions\n", file);
        fclose(f);
        return false;
    }

    swiWaitForVBlank();
    bgHide(bg);

    // Load the image scaled down to 50%, so that it fits in our allocated background
    // (with the resolution halved the output image ends up being 320x240)
    res = jd_decomp(&jpg_state, outfunc, 1);
    fclose(f);

    if(res != JDR_OK)
    {
        printf("Failed to decode image %s\n (%d)\n", file, res);
        return false;
    }

    swiWaitForVBlank();
    bgShow(bg);

    return true;
}

void show_details(const char* file)
{
    char* cwd = getcwd(NULL, 0);
    consoleClear();
    printf("Slideshow!!\n\n");
    printf("D-PAD: pan\n");
    printf("L/R: next image\n");
    printf("START: return to loader\n");
    printf("\n\nDisplaying:\n%s/%s\n", cwd, file);
    free(cwd);
}

int main()
{
    if(!isDSiMode())
    {
        consoleDemoInit();
        printf("This homebrew is only for DSi\n");
        wait_forever();
    }
    videoSetMode(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_BG);

    // allocate a 16 bitpp 512x256 background, of which we'll
    // only use a 320x240 area
    bg = bgInit(2, BgType_Bmp16, BgSize_B16_512x256, 0, 0);
    backbuffer = bgGetGfxPtr(bg);
    bgHide(bg);

    consoleDemoInit();

    // we ignore fatInitDefault because we don't need the sd
    bool ret = fatInitDefault();
    (void)ret;

    if(!nandInit(true))
    {
        printf("Nand init failed\n");
        wait_forever();
    }

    dcim_dirp = opendir(DCIM);
    if(dcim_dirp == NULL)
        goto no_pics;

    int sx = 0;
    int sy = 0;
    bool new_image = true;
    while (1)
    {
        if(new_image)
        {
            const char* filename = get_next_picture();
            if(filename == NULL)
                goto no_pics;

            if(!showImage(filename))
            {
                printf("Failed to display a picture\nAborting\n");
                wait_forever();
            }
            show_details(filename);
            new_image = false;
        }

        scanKeys();
        u16 keys_held = keysHeld();

        if(keys_held & KEY_UP)
            sy -= SCROLL_INCREMENT;

        if(keys_held & KEY_DOWN)
            sy += SCROLL_INCREMENT;

        if(keys_held & KEY_LEFT)
            sx -= SCROLL_INCREMENT;

        if(keys_held & KEY_RIGHT)
            sx += SCROLL_INCREMENT;

        if(keys_held & (KEY_L | KEY_R))
            new_image = true;

        if(sx < 0)
            sx = 0;
        else if(sx >= (BG_WIDTH - 256))
            sx = BG_WIDTH - 1 - 256;

        if(sy < 0)
            sy = 0;
        else if(sy >= (BG_HEIGHT - 192))
            sy = BG_HEIGHT - 1 - 192;

        swiWaitForVBlank();

        bgSetScroll(bg, sx, sy);

        bgUpdate();
        swiWaitForVBlank();

        if (keys_held & KEY_START)
            break;
    }
    return 0;

no_pics:
    if(dcim_dirp)
        closedir(dcim_dirp);
    printf("No pictures found on the console\nGo take some!\n");
    wait_forever();

    return 0;
}
