# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023-2024 Antonio Niño Díaz

# Base image with build tools and the ARM cross compiler
# ======================================================

FROM ubuntu:24.04 AS base-cross-compiler

ARG BRANCH=master

# Set TARGETARCH e.g. amd64, arm64, riscv64.
ARG TARGETARCH

# Set locale for iconv, used by ndstool
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        ca-certificates curl git make

RUN mkdir -p /opt/wonderful/
ADD https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz /opt/wonderful/
ADD https://wonderful.asie.pl/bootstrap/wf-bootstrap-aarch64.tar.gz /opt/wonderful/
# Note that the downloads are cached; no impact if it's a multi-platform build.
# Needed because Docker's TARGETARCH doesn't match Linux's `uname -m` format.

RUN cd /opt/wonderful/ && \
    tar xzvf wf-bootstrap-$(uname -m).tar.gz && \
    rm wf-bootstrap-*.tar.gz

ENV PATH=/opt/wonderful/bin:$PATH

# TODO: This is a workaround for pacman that generates the file /etc/mtab. Maybe
# there is a better way to fix it.
RUN cd etc && \
    ln -sf ../proc/self/mounts mtab

# Install ARM toolchain in both images
RUN wf-pacman -Syu --noconfirm && \
    wf-pacman -Syu wf-tools --noconfirm && \
    wf-config repo enable blocksds && \
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
RUN if [ $TARGETARCH = "amd64" ]; then \
        wf-pacman -S --noconfirm toolchain-llvm-teak-llvm; fi

WORKDIR /opt/
RUN git clone --recurse-submodules https://github.com/blocksds/sdk.git \
              --branch $BRANCH

WORKDIR /opt/sdk/
RUN BLOCKSDS=$PWD make install -j`nproc` VERBOSE=1 && \
    mkdir /opt/blocksds/external

WORKDIR /work/

# Add some envirnoment variables explicitly for third-party tools that don't
# default to their "official" paths when they aren't set. This needs to be set
# after the SDK is built.
ENV BLOCKSDS=/opt/blocksds/core
ENV BLOCKSDSEXT=/opt/blocksds/external
ENV WONDERFUL_TOOLCHAIN=/opt/wonderful

# Slim image
# ==========
#
# Minimalistic image with the bare minimum tools to build NDS programs with
# BlocksDS. The source code of BlocksDS isn't included.

FROM base-cross-compiler AS blocksds-slim

COPY --from=blocksds-dev /opt/blocksds/ /opt/blocksds/

WORKDIR /work/

ENV BLOCKSDS=/opt/blocksds/core
ENV BLOCKSDSEXT=/opt/blocksds/external
ENV WONDERFUL_TOOLCHAIN=/opt/wonderful
