---
title: 'Options'
weight: 0
---

## Windows

If you are using MinGW, WSL, or any similar environment, you can follow the
Linux instructions, which give you more flexibility.

If you want to install BlocksDS natively, check the instructions
[here](windows).

## Linux

If you are using Linux, MinGW, WSL, or any similar environment, follow the
instructions [here](linux).

## Docker

The Docker images of BlocksDS can be used in any OS. They require more disk
space than the native installation, but they may be needed if a native
installation isn't possible.

Follow the instructions [here](docker).

## Build from source

This is the option you should use if you want to help develop BlocksDS, or if
you want to always use the cutting edge version of BlocksDS. This is only
available in Linux-like environment.

Follow the instructions [here](from_source).

## macOS

macOS is not officially supported at this time, as BlocksDS's development team
lacks the hardware necessary to maintain and validate BlocksDS builds for the
platform. In addition, the Wonderful toolchain is only officially available
on Windows- and Linux-based platforms. For the time being, it should be
possible to make use of the Docker images on this platform, as described
above. We're looking for help to add this support, you can see more information
[here](https://github.com/blocksds/sdk/issues/205).

As an alternative, the [devkitARM toolchain](https://devkitpro.org/) provides
active support for macOS, allowing for development of homebrew software on
this operation system.
