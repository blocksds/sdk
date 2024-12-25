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

# Install SDK, then clear the package cache
RUN wf-pacman -Syu --noconfirm && \
    wf-pacman -Syu --noconfirm wf-tools && \
    wf-config repo enable blocksds && \
    wf-pacman -Syu --noconfirm && \
    wf-pacman -S --noconfirm blocksds-toolchain

# Setup the environment variables for a wf-pacman installation
ENV BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core
ENV BLOCKSDSEXT=/opt/wonderful/thirdparty/blocksds/external
ENV WONDERFUL_TOOLCHAIN=/opt/wonderful

# Use /work/ as working directory of this container.
WORKDIR /work/

# Development image
# =================

# This image contains all the packages provided by BlocksDS.

FROM base-cross-compiler AS blocksds-dev

# Install all additional BlocksDS packages
RUN wf-pacman -S --noconfirm blocksds-default blocksds-extra

# Install Teak toolchain only in the dev image
RUN if [ $TARGETARCH = "amd64" ]; then \
        wf-pacman -S --noconfirm toolchain-llvm-teak-llvm ; \
    fi

# Slim image
# ==========

# This image only contains the base SDK without examples. This is useful for
# pipelines that need to be as fast as possible.

FROM base-cross-compiler AS blocksds-slim
