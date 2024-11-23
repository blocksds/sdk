# BlocksDS Documentation

The documentation of BlocksDS is built with [Hugo](https://gohugo.io). The theme
used is [Geekdoc](https://github.com/thegeeklab/hugo-geekdoc).

The documentation of the libraries included in BlocksDS is built with
[Doxygen](https://www.doxygen.nl).

## Prerequisites

- **Hugo**

  Follow the [installation guide](https://gohugo.io/categories/installation/).

- **Geekdoc**

  Install the theme by downloading [this tarball](https://github.com/thegeeklab/hugo-geekdoc/releases/latest/download/hugo-geekdoc.tar.gz)
  and extracting it in `themes/hugo-geekdoc/`. For example, in Linux:

  ```bash
  cd docs
  wget https://github.com/thegeeklab/hugo-geekdoc/releases/latest/download/hugo-geekdoc.tar.gz | tar -xz -C themes/hugo-geekdoc/ --strip-components=1
  ```

## Testing the documentation of BlocksDS

If you have made some change to the documentation and want to check the output
locally, simply run the following command from the `docs` folder:

```bash
hugo server
```

That command will print a URL that you can open from a browser to see the
results.

You can also run the following command to only generate the static website
without a web server:

```bash
hugo
```

## Building documentation of libraries

Install Doxygen. Then, go to the root of each one of the libraries (`libnds`,
`dswifi`, etc) and run:

```bash
make docs
```

The output will be generated in the `docs/html` folder.

## Building and updating the combined documentation

The script `build-docs.sh` downloads the theme and builds the documentation of
all repositories of BlocksDS. Note that this sets the base URL of the
documentation to `https://blocksds.github.io/docs/`, so this script isn't useful
for non-maintainers.

Then, maintainers can call `publish-docs.sh` to update the folder
`blocksds-docs` (which should contain the
[docs repository](https://github.com/blocksds/docs). It creates a new commit
with the current documentation and pushes it as a new commit.
