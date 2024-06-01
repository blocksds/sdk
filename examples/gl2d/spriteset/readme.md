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

Originally, GL2D expected the user to use Texture Packer by Relminator
(http://rel.phatcode.net/junk.php?id=106). This tool is Windows-only, and it
isn't written in C so it would add more dependencies to BlocksDS (BlocksDS
requires all tools to be buildable by the user as a rule).

This example uses squeezerw (https://github.com/huxingyi/squeezer) instead. It
has been forked to export C and H files in a format similar to Texture Packer.
