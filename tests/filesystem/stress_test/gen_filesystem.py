#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

import hashlib
import os
import random
import shutil

NUM_FOLDERS = 20
NUM_FILES = 100

def md5hash(_bytes):
    m = hashlib.md5()
    m.update(_bytes)
    return m.hexdigest()

def gen_file(directory_path, size):
    _bytes = bytearray(os.urandom(size))
    file_name = md5hash(_bytes)
    full_path = os.path.join(directory_path, file_name)

    with open(full_path, "wb") as f:
        f.write(_bytes)

def gen_tree(base_path):
    folder_paths = [base_path]

    # Generate all directories. They are created in any of the directories that
    # have already been generated.
    for num in range(NUM_FOLDERS):
        name = f"dir{num}"

        num_dirs = len(folder_paths)
        destination_dir = folder_paths[random.randint(0, num_dirs - 1)]

        new_folder_path = os.path.join(destination_dir, name)
        folder_paths.append(new_folder_path)
        os.mkdir(new_folder_path)

    for num in range(NUM_FILES):
        destination_dir = folder_paths[random.randint(0, NUM_FOLDERS - 1)]
        size = random.randint(1, 256) * 1024
        gen_file(destination_dir, size)

if __name__ == "__main__":
    base = "nitrofs"

    if os.path.isdir(base):
         shutil.rmtree(base)
    os.mkdir(base)
    gen_tree(base)
