---
title: 'BlocksDS release process'
weight: 6
---

### 1. Introduction

This document describes how new BlocksDS releases are created. This information
is only useful for maintainers, but it can also be interesting for regular users
to understand the process.

### 2. Steps

There are several manual steps that you need to do, but you don't need to build
anything on your machine. All the build is done in GitHub Actions to help trace
the origins of each component that is built.

#### 2.1 Update changelog

Before doing anything, you need to ensure that the changelog is updated with the
changes that have happened since the previous release. The changelog can be
found in `docs/content/docs/changelog.md`. You will also need to decide the new
version number for the release at this point.

#### 2.2 Run some examples and tests

You should be doing this regularly, anyway, but it's always a good idea to build
and run a few tests to make sure that things aren't broken in an obvious way.

To run the automated tests, you need to setup your environment if you haven't
done it before:

```bash
bash scripts/tests-setup-environment.sh
```
Now run the automated tests. You need to run the following script from the root
of the SDK repository:

```bash
bash scripts/tests-run.sh
```

There are some tests and examples that can't be automated. You should run them
manually if if you consider it necessary. For example:

- Audio examples: `examples/audio`, `examples/maxmod`, `examples/libxm7`
- Local multiplayer examples: `examples/dswifi/local_multiplayer` and
  `examples/dswifi/multiplayer_packets`
- Microphone example: `examples/audio/microphone`
- DSi camera: `examples/peripherals/camera`
- Data cache operations test: `tests/cache/data_cache_ops`
- Slot-2 peripherals (other than RAM): `examples/peripherals/slot2`
- DSP examples: `examples/dsp`
- Atheros examples: `examples/atheros`

There are also some tests that have been automated, but benefit strongly from
being tested on hardware:

- DSWiFi (WiFi hardware emulation isn't perfect): `examples/dswifi/get_website_libcurl`
- Filesystem (NAND access is tricky): `examples/filesystems/all_filesystems`
- Exit to loader (different loaders work in different ways): `tests/system/exit_to_loader`

Also, you should test at least `examples/filesystems/all_filesystems` in no$gba
and DeSmuME.

#### 2.3 Tag all repositories

It's only really required to tag the `blocksds/sdk` repository, but all other
submodules need to be tagged with the same version string because they may use
it to generate version strings at build time, for example.

You can do it easily by running this from the root of the repository (replacing
the version string by the new one you are going to publish):

```bash
VERSION="v1.7.2" bash scripts/tag-version-all-repos.sh
```

#### 2.4 Update pacman packages

Go to the [packages repository](https://github.com/blocksds/packages/) and
create a new commit that updates the `pkgver` string of
`blocksds-toolchain/PKGBUILD` and `blocksds-docs/PKGBUILD` to the new version.

The build process will start automatically after you push the commit to the
repository.

#### 2.5 Build documentation

Run the documentation workflow, which will build and publish the documentation
automatically: [Link](https://github.com/blocksds/blocksds.github.io/actions/workflows/build.yml)

You can build the documentation at the same time that pacman packages are being
built. Also, you can build the documentation at any point in between releases
(if there are important bugs, for example).

#### 2.6 Build Docker images

**Wait until the packages are built**, the repository is updated, and the
workflow has finished successfully. Then, run the following workflow **with the
version tag that you have decided to use**. That will build and push the Docker
images: [Link](https://github.com/blocksds/sdk/actions/workflows/docker.yml)

This workflow uses the packages built right before, that's why you need to wait
for that workflow to be finished.

Note that this workflow needs the `DOCKERHUB_USERNAME` and `DOCKERHUB_TOKEN`
secrets to be setup in the repository configuration to be able to push the
images to Docker Hub: https://github.com/blocksds/sdk/settings/secrets/actions

You can also run this workflow with "latest" as target, which will update the
Docker images tagged as "latest" with the current version of the SDK in git.

#### 2.7 Create a new release in the SDK repository

Go to https://codeberg.org/blocksds/sdk/releases and create a new release by
copying the changelog of the new version. Make sure to remove the line breaks at
column 80 so that the changelog looks nicer in this web UI.

#### 2.8 Send messages on Discord

Create a short message that describes the main changes of the new version and
post it in the BlocksDS channels on Discord.
