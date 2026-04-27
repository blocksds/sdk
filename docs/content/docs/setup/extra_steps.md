---
title: 'Post-installation steps'
weight: 5
---

### 1. Test the installation

To test that everything is working, you can try building one of the
[examples](https://codeberg.org/blocksds/sdk/src/branch/master/examples) or
[project templates](https://codeberg.org/blocksds/sdk/src/branch/master/templates)
included in BlocksDS.

For example, go to the ARM9-only template folder and try to build it:

```bash
cd templates/rom_arm9_only
make
```

### 2. Check the tutorial of BlocksDS

BlocksDS has a [tutorial](../../../tutorial) that shows you how to use libnds to
use all features of the Nintendo DS. If you're new to the Nintendo DS, this is a
good way to see what kind of things you can do.

### 3. Additional libraries and tools

There are several libraries and tools that aren't installed by default. They are
listed in the links you can find [here](../../libs). They all get installed to
`BLOCKSDSEXT`.

### 4. Keeping multiple versions of BlocksDS

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
