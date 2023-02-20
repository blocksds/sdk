# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

# Base image with make and the ARM cross compiler
FROM debian:11-slim AS base-cross-compiler

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        gcc-arm-none-eabi libnewlib-arm-none-eabi make

# Image that contains all the code and the build results. This can be used to
# develop applications with BlocksDS, or to develop the components of BlocksDS
# itself.
FROM base-cross-compiler AS blocksds-dev

RUN apt-get install -y --no-install-recommends \
        build-essential ca-certificates git libfreeimage-dev libtool m4 automake

WORKDIR /opt/blocksds/
RUN git clone --recurse-submodules https://github.com/blocksds/sdk.git
ENV BLOCKSDS=/opt/blocksds/sdk/

WORKDIR /opt/blocksds/sdk/
RUN make -j`nproc`

WORKDIR /work/

# Intermediate image to extract the built SDK to some other folder
FROM blocksds-dev AS installed-sdk

WORKDIR /opt/blocksds/sdk/
RUN make INSTALLDIR=/build/sdk/ -j`nproc` install

# Minimalistic image with the bare minimum tools to build NDS programs with
# BlocksDS. The source code of BlocksDS isn't included.
FROM base-cross-compiler AS blocksds-slim

RUN apt-get install -y --no-install-recommends \
    libfreeimage3

COPY --from=installed-sdk /build/ /opt/blocksds/
ENV BLOCKSDS=/opt/blocksds/sdk/

WORKDIR /work/
