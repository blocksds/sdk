// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

// This example uses a few C++ features that hopefully test whether the
// linkerscript and crt0 setup the C++ runtime correctly.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <vector>

#include <filesystem.h>
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

void vector_test(void)
{
    std::vector<int> my_vector{123, 12, 6, 234, 6};
    my_vector.push_back(55);

    for(auto i : my_vector)
    {
        printf("%i ", i);
    }
    printf("\n");
}

// ------------------------------------------------------------------

void string_test(void)
{
    std::string hello = "hello";
    std::string world = "world";

    std::string greeting = "[std::string] " + hello + " " + world + "!";

    printf("%s ", greeting.c_str());
    printf("\n");
}

// ------------------------------------------------------------------

void new_test(void)
{
    float *p = new float[]{1.5, 2, 3.1};

    printf("new: %.2f %.2f %.2f\n", p[0], p[1], p[2]);

    delete[] p;
}

// ------------------------------------------------------------------

void fcat(const char *path)
{
    printf("cat: %s\n", path);

    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        perror("fopen");
        return;
    }

    int ret = fseek(f, 0, SEEK_END);
    if (ret != 0)
    {
        perror("fseek");
        return;
    }

    long size = ftell(f);
    if (size == -1)
    {
        perror("ftell");
        return;
    }

    printf("Size: %ld bytes\n", size);

    rewind(f);

    char *buffer = (char *)malloc(size + 1);
    if (buffer == NULL)
    {
        printf("malloc(): Not enough memory (%ld)\n", size);
        return;
    }

    size_t bytes = fread(buffer, 1, size, f);
    if (bytes != (size_t)size)
    {
        printf("fread(%ld): %zu bytes read", size, bytes);
        return;
    }
    buffer[size - 1] = '\0';

    printf("[%s]\n", buffer);

    int res = fclose(f);
    if (res != 0)
    {
        perror("fclose");
        return;
    }

    free(buffer);
}

void filesystem_test(void)
{
    // Using C++ functions

    std::cout << "Opening 'file.txt'...\n";

    std::string line;
    std::ifstream myfile("file.txt");
    if (myfile.is_open())
    {
        std::cout << "Read file: [";

        while (getline(myfile, line))
            std::cout << line << '\n';
        myfile.close();

        std::cout << "]\n";
    }
    else
    {
        std::cout << "Failed to open";
    }

    // Using C functions

    std::cout << "Opening 'file.txt'...\n";

    fcat("file.txt");
}

// ------------------------------------------------------------------

int main(int argc, char *argv[])
{
    consoleDemoInit();

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        goto exit;
    }

    chdir("nitro:/");

    // Run some C++ tests

    virtual_functions_test();
    global_constructor_test();
    vector_test();
    string_test();
    new_test();
    filesystem_test();

exit:
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
