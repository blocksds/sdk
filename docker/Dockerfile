# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

# Base image with build tools and the ARM cross compiler
# ======================================================

FROM ubuntu:23.04 AS base-cross-compiler

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        ca-certificates make dosfstools mtools

RUN mkdir -p /opt/wonderful/
ADD https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz /opt/wonderful/
RUN cd /opt/wonderful/ && \
    tar xzvf wf-bootstrap-x86_64.tar.gz && \
    rm wf-bootstrap-x86_64.tar.gz

ENV PATH /opt/wonderful/bin:$PATH

# TODO: This is a workaround for pacman that generates the file /etc/mtab. Maybe
# there is a better way to fix it.
RUN cd etc && \
    ln -sf ../proc/self/mounts mtab

RUN wf-pacman -Syu --noconfirm && \
    wf-pacman -S --noconfirm \
        toolchain-gcc-arm-none-eabi-binutils \
        toolchain-gcc-arm-none-eabi-gcc \
        toolchain-gcc-arm-none-eabi-gcc-libs \
        toolchain-gcc-arm-none-eabi-libstdcxx-picolibc \
        toolchain-gcc-arm-none-eabi-picolibc-generic

ENV PATH /opt/wonderful/toolchain/gcc-arm-none-eabi/bin/:$PATH

# Full development image
# ======================
#
# Image that contains all the code and the build results. This can be used to
# develop applications with BlocksDS, or to develop the components of BlocksDS
# itself.

FROM base-cross-compiler AS blocksds-dev

RUN apt-get install -y --no-install-recommends \
        build-essential git libfreeimage-dev meson ninja-build

WORKDIR /opt/blocksds/
RUN git clone --recurse-submodules https://github.com/blocksds/sdk.git
ENV BLOCKSDS=/opt/blocksds/sdk/

WORKDIR /opt/blocksds/sdk/
RUN make -j`nproc` VERBOSE=1

WORKDIR /work/

# Intermediate image to extract the built SDK to some other folder
FROM blocksds-dev AS installed-sdk

WORKDIR /opt/blocksds/sdk/
RUN make INSTALLDIR=/build/sdk/ -j`nproc` install

# Slim image
# ==========
#
# Minimalistic image with the bare minimum tools to build NDS programs with
# BlocksDS. The source code of BlocksDS isn't included.

FROM base-cross-compiler AS blocksds-slim

RUN apt-get install -y --no-install-recommends \
    libfreeimage3

COPY --from=installed-sdk /build/ /opt/blocksds/
ENV BLOCKSDS=/opt/blocksds/sdk/

WORKDIR /work/
