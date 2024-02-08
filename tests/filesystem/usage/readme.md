# Filesystem test

## Running on hardware

Copy the contents of folder `sdroot` to the root of your SD card. This test
will work on flashcarts that support DLDI, and in the internal SD slot in a DSi
running in DSi mode.

The resulting structure should look like this:

```
SD root
|
|- Other files
| ...
\- fstest
   |- d1
   |  | ...
   |  \ ...
   |- d2
   |  |- d2_remove.txt
   |- f1.txt
   |- f2.txt
   \- long_file.txt
```

## Running on emulators

Check the [filesystem documentation](https://blocksds.github.io/docs/technical/filesystem/)
for more information.
