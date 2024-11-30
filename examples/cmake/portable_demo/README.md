## Portable Demo

This project provides an example application that builds for both NDS and POSIX (computer) platforms.

### Usage

To build an executable native to your host system:
```
mkdir -p build/POSIX
cd build/POSIX
cmake ../..
make
```

To build an executable for NDS:
```
mkdir -p build/NDS
cd build/NDS
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDS.cmake
make
```

To build a DSi-exclusive executable:
```
mkdir -p build/DSi
cd build/DSi
cmake ../.. -DCMAKE_TOOLCHAIN_FILE=$BLOCKSDS/cmake/BlocksDSi.cmake
make
```

Note that such an executable may violate the NDS ROM specifications and fail to load in some environments.
