---
title: 'Introduction'
weight: -50
---

This is an SDK for Nintendo DS. It builds on the work of several people, and it
has the following goals:

- To create an easy-to-use NDS software development kit based on GCC with as few
  dependencies as possible.

- To encourage developers to hack and improve the SDK by letting them have all
  the source code of the SDK. This also shows how easy it is to build the whole
  codebase. The exception is the tolchain, which is non-trivial to build.
  Because of that, [Wonderful Toolchains](https://wonderful.asie.pl/) is used
  instead of asking the users to build it themselves.

- To try to keep compatibility with pre-existing C projects. In theory, it
  should be enough to replace the makefile by one of BlocksDS to build any
  project that uses libnds.

This project is currently in beta stage, and most features are working. Please,
check the [libc port documentation](../../technical/libc) for more information
about the supported libc functions.

If you want to port a project built with devkitPro, follow
[this guide](../../technical/devkitarm_porting_guide) for instructions.

For detailed information about the design of BlocksDS, check
[this other document](../../technical/design_guide).

For licensing information, check [this](../licenses).
