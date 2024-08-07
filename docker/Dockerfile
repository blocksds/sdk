# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023-2024 Antonio Niño Díaz

# Base image with build tools and the ARM cross compiler
# ======================================================

FROM ubuntu:23.04 AS base-cross-compiler

ARG BRANCH=master

# Set locale for iconv, used by ndstool
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        ca-certificates git make

RUN mkdir -p /opt/wonderful/
ADD https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz /opt/wonderful/
RUN cd /opt/wonderful/ && \
    tar xzvf wf-bootstrap-x86_64.tar.gz && \
    rm wf-bootstrap-x86_64.tar.gz

ENV PATH=/opt/wonderful/bin:$PATH

# TODO: This is a workaround for pacman that generates the file /etc/mtab. Maybe
# there is a better way to fix it.
RUN cd etc && \
    ln -sf ../proc/self/mounts mtab

# Install ARM toolchain in both images
RUN wf-pacman -Syu --noconfirm && \
    wf-pacman -S --noconfirm toolchain-gcc-arm-none-eabi

# Full development image
# ======================
#
# Image that contains all the code and the build results. This can be used to
# develop applications with BlocksDS, or to develop the components of BlocksDS
# itself.

FROM base-cross-compiler AS blocksds-dev

# Install host GCC so that we can build the host tools. This isn't needed in the
# slim image because it gets the pre-built binaries.
RUN apt-get install -y --no-install-recommends \
        build-essential

# Install Teak toolchain only in the dev image
RUN wf-pacman -S --noconfirm toolchain-llvm-teak-llvm

WORKDIR /opt/
RUN git clone --recurse-submodules https://github.com/blocksds/sdk.git \
              --branch $BRANCH

WORKDIR /opt/sdk/
RUN BLOCKSDS=$PWD make install -j`nproc` VERBOSE=1 && \
    mkdir /opt/blocksds/external

WORKDIR /work/

# Slim image
# ==========
#
# Minimalistic image with the bare minimum tools to build NDS programs with
# BlocksDS. The source code of BlocksDS isn't included.

FROM base-cross-compiler AS blocksds-slim

COPY --from=blocksds-dev /opt/blocksds/ /opt/blocksds/

WORKDIR /work/
