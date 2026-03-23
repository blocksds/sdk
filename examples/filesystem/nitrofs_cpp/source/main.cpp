// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

#include <filesystem.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

static PrintConsole topScreen;
static PrintConsole bottomScreen;

void wait_press_button_a(void)
{
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_A)
            break;
    }
}

void wait_forever(void)
{
    while (1)
        swiWaitForVBlank();
}

void dir_list(void)
{
    consoleClear();

    std::cout << "path: " << std::filesystem::current_path() << std::endl << std::endl;

    const std::filesystem::path path{"."};
    std::error_code ec;
    std::filesystem::directory_iterator di{path, ec};
    if (ec)
    {
        std::cout << "\e[91m" << __func__ << ": " << ec.message() << "\e[m\n";
        wait_forever();
    }

    int num_entries = 0;

    for (auto const& dir_entry : di)
    {
        std::cout << num_entries << " - " << dir_entry.path() <<
            (dir_entry.is_directory() ? " DIR" : " ") << std::endl;

        num_entries++;
    }

    consoleSetCursor(NULL, 0, 23);
    std::cout << "Num entries: " << num_entries;

    wait_press_button_a();

    consoleClear();
}

void fcat(const char *path)
{
    std::cout << "fcat: " << path << std::endl << std::endl;

    std::ifstream f(path);

    if (!f.is_open())
    {
        std::cout << "Failed to open " << path << std::endl;
        wait_forever();
    }

    std::string s;

    std::cout << "[";
    while (std::getline(f, s))
        std::cout << s << std::endl;
    std::cout << "]";

    f.close();
    return;
}

void change_working_dir(const char *path)
{
    std::error_code ec;
    std::filesystem::path filePath(path);
    std::filesystem::current_path(filePath, ec);

    if (ec)
    {
        std::cout << "\e[91m" << __func__ << ": " << ec.message() << "\e[m\n";
        wait_forever();
    }
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    std::cout << "DLDI name:" << std::endl;
    std::cout << io_dldi_data->friendlyName << std::endl;
    std::cout << std::endl;
    std::cout << "DSi mode: " << isDSiMode() << std::endl;
    std::cout << std::endl;
    std::cout << "argc: " << argc << std::endl;
    if (argc > 0)
        std::cout << "argv[0]: " << argv[0] << std::endl;
    std::cout << std::endl;

    // It isn't needed to call fatInitDefault() manually. If nitroFSInit detects
    // that the ROM is running in a flashcard or from the DSi internal SD slot,
    // it will call it internally.
    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    consoleSelect(&bottomScreen);

    // Change the current working directory to the base directory used for
    // testing.

    change_working_dir("nitro:/fstest");
    std::cout << "Current dir: " << std::filesystem::current_path() << std::endl << std::endl;
    wait_press_button_a();

    // Print the contents of multiple directories.

    change_working_dir("d1");
    dir_list();

    change_working_dir("d1d1");
    dir_list();

    change_working_dir("../d1d2");
    dir_list();

    change_working_dir("../..");
    dir_list();

    // Print contents of one file

    fcat("long_file.txt");
    wait_press_button_a();

    // Done!

    consoleClear();
    std::cout << "All test passed!" << std::endl;
    std::cout << std::endl;
    std::cout << "Press START to exit to loader" << std::endl;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
