This example shows how to combine multiple PNG files of arbitrary sizes into a
single texture to be loaded by your application. This allows you to use VRAM
more efficiently (you can use a single texture with a valid power of two size
formed by many smaller images that don't have a power of two size).

To regenerate the combined texture, run:
```sh
sh generate_assets.sh
```

Then build the example as usual:
```sh
make
```
