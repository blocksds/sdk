// SPDX-License-Identifier: MIT
//
// Copyright (c) 2014, 2019-2020, 2023 Antonio Niño Díaz

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN    2048

char out_array_name[MAX_PATH_LEN];
char c_file_name[MAX_PATH_LEN];
char h_file_name[MAX_PATH_LEN];

void file_load(const char *path, void **buffer, size_t *size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        printf("%s couldn't be opened!", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    *size = ftell(f);

    if (*size == 0)
    {
        printf("Size of %s is 0!", path);
        fclose(f);
        exit(1);
    }

    rewind(f);
    *buffer = malloc(*size);
    if (*buffer == NULL)
    {
        printf("Not enought memory to load %s!", path);
        fclose(f);
        exit(1);
    }

    if (fread(*buffer, *size, 1, f) != 1)
    {
        printf("Error while reading.");
        fclose(f);
        exit(1);
    }

    fclose(f);
}

// Converts a file name to an array name and output file names
void generate_transformed_name(const char *path, const char *dir_out)
{
    size_t len = strlen(path);

    size_t start = 0;

    for (int i = len - 1; i > 0; i--)
    {
        if (path[i] == '/')
        {
            start = i + 1;
            break;
        }
    }

    const char *basename = &(path[start]);

    snprintf(c_file_name, sizeof(c_file_name), "%s/%s.c", dir_out, basename);
    snprintf(h_file_name, sizeof(h_file_name), "%s/%s.h", dir_out, basename);

    // Fix names of header and C files and replace '.bin' by '_bin'.
    //
    //     out.path/34.my.bin.file.bin -> out.path/34.my.bin.file_bin.h
    //                                    out.path/34.my.bin.file_bin.c
    {
        len = strlen(h_file_name);

        for (size_t i = len - 3; i > 0; i--)
        {
            if (h_file_name[i] == '.')
            {
                h_file_name[i] = '_';
                c_file_name[i] = '_';
                break;
            }
            else if (h_file_name[i] == '/')
            {
                break;
            }
        }
    }

    // Create the array name. If the name begins by a digit, prefix it with a
    // '_' to form a valid identifier. Also, convert dots and dashes to
    // underscores.
    //
    //     my-bin-file.bin -> my_bin_file_bin[]
    //     34.my.bin.file.bin -> _34_my_bin_file_bin[]
    {
        const char *prefix = "";
        if (isdigit(path[start]))
            prefix = "_";

        snprintf(out_array_name, sizeof(out_array_name), "%s%s",
                 prefix, &(path[start]));

        len = strlen(out_array_name);

        for (size_t i = 0; i < len; i++)
        {
            if ((out_array_name[i] == '.') || (out_array_name[i] == '-'))
                out_array_name[i] = '_';
        }
    }
}

int main(int argc, char **argv)
{
    void *file = NULL;
    size_t size;

    if (argc < 3)
    {
        printf("Invalid arguments.\n"
               "Usage: %s [file_in] [folder_out]\n", argv[0]);
        return 1;
    }

    const char *path_in = argv[1];
    const char *dir_out = argv[2];

    file_load(path_in, &file, &size);

    generate_transformed_name(path_in, dir_out);

    FILE *fc = fopen(c_file_name, "w");
    if (fc == NULL)
    {
        printf("Can't open %s\n", c_file_name);
        return 1;
    }

    const char *c_header =
        "// Autogenerated file. Do not edit.\n"
        "\n"
        "#include <stdint.h>\n"
        "\n";

    fprintf(fc, "%s", c_header);
    fprintf(fc, "const uint8_t %s[%zu] __attribute__((aligned(4)))  =\n",
            out_array_name, size);
    fprintf(fc, "{\n");

    uint8_t *data = file;

    for (size_t i = 0; i < size; i++)
    {
        if ((i % 12) == 0)
            fprintf(fc, "    ");

        fprintf(fc, "0x%02X", *data);
        data++;

        if (i == size - 1)
            fprintf(fc, "\n");
        else if ((i % 12) == 11)
            fprintf(fc, ",\n");
        else
            fprintf(fc, ", ");
    }

    fprintf(fc, "};\n");

    fclose(fc);

    FILE *fh = fopen(h_file_name, "w");
    if (fh == NULL)
    {
        printf("Can't open %s\n", h_file_name);
        return 1;
    }

    fprintf(fh,
        "// Autogenerated file. Do not edit.\n"
        "\n"
        "#pragma once\n"
        "\n"
        "#include <stdint.h>\n"
        "\n"
        "#define %s_size (%zu)\n"
        "extern const uint8_t %s[%zu];\n",
        out_array_name, size,
        out_array_name, size);

    fclose(fh);

    free(file);

    return 0;
}
