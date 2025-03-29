# BlocksDS Tutorial

The tutorial of BlocksDS is built with [Hugo](https://gohugo.io). The theme
used is [Geekdoc](https://github.com/thegeeklab/hugo-geekdoc).

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

## Testing the tutorial of BlocksDS

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

## Building the tutorial

The script `build-docs.sh` downloads the theme and builds the tutorial. Note
that this sets the base URL of the documentation to
`https://blocksds.skylyrac.net/tutorial/`, so this script isn't useful for
non-maintainers. This script is used by the GitHub Actions workflow of the
following repository: https://github.com/blocksds/tutorial
