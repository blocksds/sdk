---
title: 'Post-installation steps'
weight: 5
---

### 1. Test the installation

To test that everything is working, you can try building one of the
[examples](https://github.com/blocksds/sdk/tree/master/examples) or
[project templates](https://github.com/blocksds/sdk/tree/master/templates)
included in BlocksDS.

There is a template that lets the programmer write the code for both CPUs of the
NDS (`rom_combined`) and a template that uses the default ARM7 core that comes
with Blocks DS and lets the programmer focus on the ARM9 core (`rom_arm9`).
There are also two templates to create static libraries.

For example, go to the combined template folder and build it:

```bash
cd templates/rom_combined
make
```

This should have generated a NDS ROM that you can run on any emulator or
flashcart.

To do a verbose build, run `make` like this (this also works for the Makefile of
the SDK):

```bash
VERBOSE=1 make
```

**NOTE:** The paths in the Makefile that refer to source code, includes,
graphics, data, etc, must be inside the folder of the project. That means you
can't use `..` in a path to go one level up from the Makefile. If you really
need to use folders outside of the folder of the project, create a symlink to
the destination, or build the other code as a static library and link it with
the project.

### 2. Additional libraries and tools

There are several libraries and tools that aren't installed by default. They are
listed in the links you can find [here](../../docs). They all get installed to
`BLOCKSDSEXT`.

### 3. Keeping multiple versions of BlocksDS

You may keep multiple versions of this SDK in your PC. The location of the
active SDK is expected to be in the environment variables `BLOCKSDS` and
`BLOCKSDSEXT`, so all you need to do is to change their values and point to the
version of the SDK you want to use.

`BLOCKSDS` is used for core BlocksDS libraries and tools, and it is managed by
BlocksDS. `BLOCKSDSEXT` is left for users to freely install third party
libraries and tools.

If they aren't set the default makefiles assume that the paths are
`/opt/blocksds/core/` and `/opt/blocksds/external/`. Other tools should imitate
this behaviour.
