# BlocksDS Documentation

The documentation of BlocksDS is built with [Hugo](https://gohugo.io). The theme
used is [Geekdoc](https://github.com/thegeeklab/hugo-geekdoc).

The documentation of the libraries included in BlocksDS is built with
[Doxygen](https://www.doxygen.nl).

## Prerequisites

- **Hugo**

  Follow the [installation guide](https://gohugo.io/categories/installation/).
  You need the extended version of Hugo.

- **Hextra**

  Download the theme by cloning the repository in `themes/hextra`:

  ```bash
  cd themes
  git clone https://github.com/imfing/hextra --depth=1
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
documentation to `https://blocksds.skylyrac.net/`, so this script isn't useful
for non-maintainers. This script is used by the GitHub Actions workflow of the
following repository: https://codeberg.org/blocksds/docs
