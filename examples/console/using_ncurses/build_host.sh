#!/bin/sh

# This script builds the example for the host. You will need to install the
# development package of ncurses. For exaxmple:
#
#     sudo apt install libncurses-dev

gcc -Wall -Wextra -o main source/main.c -lncurses
