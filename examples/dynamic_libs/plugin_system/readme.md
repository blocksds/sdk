# Application with a plugin system

This example shows how to load plugins to an application even though they have
been generated outside of the source tree without access to any information of
the application.

This example builds two plugins, copies their binaries to the `nitrofs` folder
of the application, and then builds the application. You don't have to use
NitroFS for this system to work, you could load the plugins from individual
files in the SD card of the flashcard (or DSi slot). However, to make the
example easier to build and test, the example uses NitroFS.

The build system used is [ArchitectDS](https://codeberg.org/blocksds/architectds)
because of the flexibility it gives the user. However, it is needed to run it
three times (once for each plugin, once for the application).

In order to build this example easily, run:

```bash
bash build.sh
```

To clean it:

```bash
bash clean.sh
```
