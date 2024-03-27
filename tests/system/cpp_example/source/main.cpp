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

unsigned int virtual_test_flags = 0;

class base
{
public:
    base()
    {
        printf("constructor: base class\n");
        virtual_test_flags |= BIT(0);
    }

    ~base()
    {
        printf("destructor: base class\n");
        virtual_test_flags |= BIT(1);
    }

    virtual void print()
    {
        printf("print: base class\n");
        virtual_test_flags |= BIT(2);
    }

    void show()
    {
        printf("show: base class\n");
        virtual_test_flags |= BIT(3);
    }
};

class derived : public base
{
public:
    derived()
    {
        printf("constructor: derived class\n");
        virtual_test_flags |= BIT(4);
    }

    ~derived()
    {
        printf("destructor: derived class\n");
        virtual_test_flags |= BIT(5);
    }

    void print()
    {
        printf("print: derived class\n");
        virtual_test_flags |= BIT(6);
    }

    void show()
    {
        printf("show: derived class\n");
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
    printf("Flags: %X\n", virtual_test_flags);

    if (virtual_test_flags != 0x7B)
    {
        printf("%s() failed\n", __func__);
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
};

global_initializer my_global_initializer;

bool global_constructor_test(void)
{
    if (global_var == 0x1234)
    {
        printf("global constructor OK\n");
        return true;
    }
    else
    {
        printf("global constructor FAIL\n");
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
            printf("%s(): Value %d (Exp: %d)\n", __func__, i, expected[counter]);
            ok = false;
        }

        counter++;
    }

    if (counter != 6)
    {
        printf("%s(): Counter %d (Exp: 6)\n", __func__, counter);
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

    printf("%s ", greeting.c_str());
    printf("\n");

    return (strcmp(greeting.c_str(), "[std::string] hello world!") == 0);
}

// ------------------------------------------------------------------

bool new_test(void)
{
    float *p = new float[]{1.5, 2, 3.1};

    printf("new: %.2f %.2f %.2f\n", p[0], p[1], p[2]);

    bool ok = false;
    if (p != NULL)
        ok = true;

    delete[] p;

    return ok;
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

bool filesystem_test(void)
{
    // Using C++ functions

    std::cout << "Opening 'file.txt'...\n";

    std::string contents = "";
    std::string line;
    std::ifstream myfile("file.txt");
    if (myfile.is_open())
    {
        std::cout << "Read file: [";

        while (getline(myfile, line))
            contents += line + "\n";

        std::cout << contents;
        myfile.close();

        std::cout << "]\n";
    }
    else
    {
        std::cout << "Failed to open";
    }

    bool ok = false;
    if (strcmp(contents.c_str(), "This is a test!\n") == 0)
        ok = true;

    // Using C functions

    std::cout << "Opening 'file.txt'...\n";

    fcat("file.txt");

    return ok;
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

    if (!virtual_functions_test())
        goto exit;

    if (!global_constructor_test())
        goto exit;

    if (!vector_test())
        goto exit;

    if (!string_test())
        goto exit;

    if (!new_test())
        goto exit;

    if (!filesystem_test())
        goto exit;

    printf("All tests passed!\n");

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
