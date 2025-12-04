---
title: 'Docker'
weight: 10
---

## 1. Introduction

This is a short and simplified explanation of Docker in case you aren't familiar
with it.

Docker is a program that lets you run containers (like small virtual machines)
on your computer. The creator of a Docker container defines the contents of that
container, and the user of that container receives the exact same contents.
This solves compatibility issues because of differences between the computers of
the developer and the user.

You can have containers running Linux programs on Windows or macOS, so it is
possible to use a Linux BlocksDS installation on Windows or macOS this way.

It is very easy to share folders from your computer with a container. That way
you can share the folder with your code (which is in a regular folder in your
computer) with the container. Then, you build the code from the container. The
result will be visible right away from outside of the container.  Then you can
test the result on an emulator or copy it to a flashcart!

The way you are expected to work with containers is to open your favourite code
editor in your operating system, and to open a terminal window with the Docker
image. You can edit the code as usual, switch to the Docker terminal window,
type `make` to build the code, and see the results in your operating system.

To install Docker, check this link: https://docs.docker.com/get-docker/

You may need to reboot after the installation.

Note: A more advanced usage of containers are automated builds. Any computer
using the same container will have the exact same programs installed in that
container, so they should all be able to build the code without compatibility
issues. This means that you can use GitHub pipelines, for example, to
automatically generate a build of your project whenever a new patch is pushed to
the repository.

## 2. BlocksDS container images

There are two different official images:

- `blocksds-slim`: (Recommended) Smaller image. For developers that just want to
  focus on developing NDS programs but don't want to modify BlocksDS.
- `blocksds-dev`: For developers that want to improve BlocksDS. It contains the
  same contents as the slim version, plus all the tools and code required to
  modify BlocksDS (the git repository of BlocksDS is in `/opt/blocksds/sdk/`).

Both images have the SDK ready to be used. You don't need to setup any
environment variable or anything else.

The images are available on architectures amd64 and arm64.

## 3. Downloading the pre-built images

The official BlocksDS container images are available
[here](https://hub.docker.com/r/skylyrac/blocksds).

Run one of the two following commands to get your desired image:

```bash
docker image pull skylyrac/blocksds:slim-latest
```

or


```bash
docker image pull skylyrac/blocksds:dev-latest
```

You will need to run the same commands whenver you want to update the image.

## 4. Build images locally

If you don't want to get the pre-built images, you can build them yourself. You
have the option to build the image for your platform, or a multi-platform image.
For a multi-platform image you need to follow the instructions in
[this page](https://docs.docker.com/desktop/containerd/#build-multi-platform-images).

Open a terminal in the `docker` folder of this repository run the following
commands, depending on the image, architecture (or architectures) and branch you
want to build:

```bash
TARGET="blocksds-slim" # Or "blocksds-dev"
TAG_NAME="blocksds:slim" # Or "blocksds:dev"
BRANCH="master"
PLATFORM="linux/amd64,linux/arm64" # Or "linux/amd64", or "linux/arm64"

docker build --target $TARGET --tag $TAG_NAME \
        --build-arg BRANCH=$BRANCH --platform $PLATFORM .
```

You will need to run the same commands whenver you want to update the image.

## 5. How to use the containers

All instructions from now on will use the slim image, but they work the same way
on the dev version.

We're going to build one of the templates of the SDK as a test.

First, clone this repository to get the templates:

```bash
git clone https://github.com/blocksds/sdk
```

Go to the folder that contains the template:

```bash
cd sdk/templates/rom_arm9_only
```

Check the full path to this folder, you will need it in the next step.

Now, open a different terminal window and run the following command to start the
BlocksDS Docker container. The command will also share a the folder of your
computer with the template. This folder will appear in path `/work` inside the
Docker container terminal.

- For pre-built images:

    ```bash
    docker run --rm -v /path/in/your/pc/sdk/templates/rom_arm9_only:/work \
        -it --entrypoint bash skylyrac/blocksds:slim-latest
    ```

- For locally built images:

    ```bash
    docker run --rm -v /path/in/your/pc/sdk/templates/rom_arm9_only:/work \
        -it --entrypoint bash blocksds:slim
    ```

Explanation:

- `--rm` removes any stopped instance of this container that may be already
  present on your computer.

- `-it` asks Docker to start an interactive terminal window

- The argument that shares a folder is `-v`. The first half of the argument
  (before `:` is the path on the host computer. The second part is the
  destination path inside the container.

- `--entrypoint bash` tells Docker to run `bash` when the container starts, so
  that you can run commands inside the container.

- The last argument is the name of the Docker image you want to use.

Now, inside the container terminal, run:

```bash
cd /work
make
```

This should build a NDS ROM. Check the folder in your operating system and the
ROM should be there.

You can close the container by running:

```bash
exit
```

## 6. Uninstall the images

If you have installed Docker just for this, you can run the following command to
remove all docker images available in your PC:

```bash
docker system prune -a
```
