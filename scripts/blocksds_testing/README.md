# BlocksDS testing module

This is a simple wrapper that sets up a headless test session with
[libretro.py](https://github.com/JesseTG/libretro.py) and
[melonDS DS](https://github.com/JesseTG/melonds-ds).

## Dependencies

You need to install Python 3. In Ubuntu or Debian simply run:

```sh
sudo apt install python3
```

You will also need to build melonDS DS yourself, or get a pre-built
`melondsds_libretro.so` file.

## Build

To build it, do it in a virtual environment:

```sh
sudo apt install python3-venv

# Create virtual environment and activate it
python3 -m venv env
source env/bin/activate

# Install dependencies
pip3 install -r requirements.txt
pip3 install build setuptools

# Create wheel
python3 -m build
```

## Installation

From a different (or the same) virtual environment, run:

```sh
pip3 install -r requirements.txt
pip3 install dist/architectds-*-py3-none-any.whl
```

## Usage

Place your melonDS DS core and all your DS/DSi NAND, firmware and BIOS dumps
into a folder (for example, `blobs`) and use the following names:

```
bios7.bin
bios7i.bin
bios9.bin
bios9i.bin
dsfirmware.bin
nand.bin
melondsds_libretro.so
```

Before using any test that imports `blocksds_testing`, set the
`BLOCKSDS_TESTING_BLOBS` envirnoment variable:

```sh
export BLOCKSDS_TESTING_BLOBS="$PWD/blobs"
```
