// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

// This example uses a few C++ features that hopefully test whether the
// linkerscript and crt0 setup the C++ runtime correctly.

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <vector>

#include <nds.h>

// ------------------------------------------------------------------

// Test virtual functions with derived classes

unsigned int virtual_test_flags = 0;

class base
{
public:
    base()
    {
        consolePrintf("constructor: base class\n");
        virtual_test_flags |= BIT(0);
    }

    ~base()
    {
        consolePrintf("destructor: base class\n");
        virtual_test_flags |= BIT(1);
    }

    virtual void print()
    {
        consolePrintf("print: base class\n");
        virtual_test_flags |= BIT(2);
    }

    void show()
    {
        consolePrintf("show: base class\n");
        virtual_test_flags |= BIT(3);
    }
};

class derived : public base
{
public:
    derived()
    {
        consolePrintf("constructor: derived class\n");
        virtual_test_flags |= BIT(4);
    }

    ~derived()
    {
        consolePrintf("destructor: derived class\n");
        virtual_test_flags |= BIT(5);
    }

    void print()
    {
        consolePrintf("print: derived class\n");
        virtual_test_flags |= BIT(6);
    }

    void show()
    {
        consolePrintf("show: derived class\n");
        virtual_test_flags |= BIT(7);
    }
};

void virtual_functions_test_inner(void)
{
    base *bptr;
    derived d;
    bptr = &d;

    // virtual function, binded at runtime
    bptr->print();

    // Non-virtual function, binded at compile time
    bptr->show();
}

bool virtual_functions_test(void)
{
    virtual_functions_test_inner();
    consolePrintf("Flags: %x\n", virtual_test_flags);

    if (virtual_test_flags != 0x7B)
    {
        consolePrintf("%s() failed\n", __func__);
        return false;
    }

    return true;
}

// ------------------------------------------------------------------

// Test constructors that are called before main()

static unsigned int global_var;

class global_initializer
{
public:
    global_initializer(void)
    {
        global_var = 0x1234;
    }

    ~global_initializer(void)
    {
        consolePrintf("global destructor called!");

        consoleFlush();

        for (int i = 0; i < 120; i++)
            swiWaitForVBlank();
    }
};

global_initializer my_global_initializer;

bool global_constructor_test(void)
{
    if (global_var == 0x1234)
    {
        consolePrintf("global constructor OK\n");
        return true;
    }
    else
    {
        consolePrintf("global constructor FAIL\n");
        return false;
    }
}

// ------------------------------------------------------------------

bool vector_test(void)
{
    std::vector<int> my_vector{123, 12, 6, 234, 6};
    my_vector.push_back(55);

    bool ok = true;
    int expected[] = {123, 12, 6, 234, 6, 55};

    int counter = 0;
    for (auto i : my_vector)
    {
        if (i != expected[counter])
        {
            consolePrintf("%s(): Value %d (Exp: %d)\n", __func__, i, expected[counter]);
            ok = false;
        }

        counter++;
    }

    if (counter != 6)
    {
        consolePrintf("%s(): Counter %d (Exp: 6)\n", __func__, counter);
        return false;
    }

    return ok;
}

// ------------------------------------------------------------------

bool string_test(void)
{
    std::string hello = "hello";
    std::string world = "world";

    std::string greeting = "[std::string] " + hello + " " + world + "!";

    consolePrintf("%s ", greeting.c_str());
    consolePrintf("\n");

    return (strcmp(greeting.c_str(), "[std::string] hello world!") == 0);
}

// ------------------------------------------------------------------

bool new_test(void)
{
    int *p = new int[]{1, 2, 3};

    consolePrintf("new: %d %d %d\n", p[0], p[1], p[2]);

    bool ok = false;
    if (p != NULL)
        ok = true;

    delete[] p;

    return ok;
}

// ------------------------------------------------------------------

volatile bool exit_loop = false;

void power_button_callback(void)
{
    exit_loop = true;
}

void vblank_handler(void)
{
    inputGetAndSend();
}

int main(int argc, char *argv[])
{
    // Initialize sound hardware
    enableSound();

    // Read user information from the firmware (name, birthday, etc)
    readUserSettings();

    // Stop LED blinking
    ledBlink(LED_ALWAYS_ON);

    // Using the calibration values read from the firmware with
    // readUserSettings(), calculate some internal values to convert raw
    // coordinates into screen coordinates.
    touchInit();

    irqInit();
    fifoInit();

    installSoundFIFO();
    installSystemFIFO(); // Sleep mode, storage, firmware...

    // This sets a callback that is called when the power button in a DSi
    // console is pressed. It has no effect in a DS.
    setPowerButtonCB(power_button_callback);

    // Read current date from the RTC and setup an interrupt to update the time
    // regularly. The interrupt simply adds one second every time, it doesn't
    // read the date. Reading the RTC is very slow, so it's a bad idea to do it
    // frequently.
    initClockIRQTimer(LIBNDS_DEFAULT_TIMER_RTC);

    // Now that the FIFO is setup we can start sending input data to the ARM9.
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    // Wait until the ARM7 console is setup before printing to it
    while (!consoleIsSetup())
        swiWaitForVBlank();

    consolePuts("===== ARM7 =====\n");

    bool passed = true;

    if (!virtual_functions_test())
        passed = false;

    if (!global_constructor_test())
        passed = false;

    if (!vector_test())
        passed = false;

    if (!string_test())
        passed = false;

    if (!new_test())
        passed = false;

    if (passed)
        consolePrintf("All tests passed!\n");
    else
        consolePrintf("Some tests failed!\n");

    consolePrintf("\n\n");
    consolePrintf("SELECT: ARM7 exit to loader\n");

    consoleFlush();

    while (!exit_loop)
    {
        const uint16_t key_mask = KEY_SELECT;
        uint16_t keys_pressed = ~REG_KEYINPUT;

        if ((keys_pressed & key_mask) == key_mask)
            exit_loop = true;

        swiWaitForVBlank();
    }

    return 0;
}
