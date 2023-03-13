// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example uses a few C++ features that hopefully test whether the
// linkerscript and crt0 setup the C++ runtime correctly.

// C++ I/O streams aren't supported
#include <cstdio>

#include <nds.h>

// ------------------------------------------------------------------

// Test virtual functions with derived classes

class base
{
public:
    base()
    {
        printf("constructor: base class\n");
    }

    ~base()
    {
        printf("destructor: base class\n");
    }

    virtual void print()
    {
        printf("print: base class\n");
    }

    void show()
    {
        printf("show: base class\n");
    }
};

class derived : public base
{
public:
    derived()
    {
        printf("constructor: derived class\n");
    }

    ~derived()
    {
        printf("destructor: derived class\n");
    }

    void print()
    {
        printf("print: derived class\n");
    }

    void show()
    {
        printf("show: derived class\n");
    }
};

void virtual_functions_test(void)
{
    base *bptr;
    derived d;
    bptr = &d;

    // virtual function, binded at runtime
    bptr->print();

    // Non-virtual function, binded at compile time
    bptr->show();
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
};

global_initializer my_global_initializer;

void global_constructor_test(void)
{
    if (global_var == 0x1234)
        printf("global constructor OK\n");
    else
        printf("global constructor FAIL\n");
}

// ------------------------------------------------------------------

int main(int argc, char *argv[])
{
    consoleDemoInit();

    // Run some C++ tests

    virtual_functions_test();
    global_constructor_test();

    printf("\n\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }
}
