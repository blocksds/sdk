ncurses example
===============

This example shows how to create a custom console and keyboard with a custom
terminfo configuration file. The resulting system can be used with ncurses.

You need to install ncurses in your system if you want to modify the console
configuration or if you want to build the example for your PC. For example,
run this in Ubuntu or Debian:

```bash
sudo apt install libncurses-dev
```

If you modify the `libnds.ti` file you will need to run `convert.sh` to compile
the file.

If you want to build the application for PC, run `build_host.sh`. The resulting
binary is `main`.
