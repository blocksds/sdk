####################
BlocksDS with Docker
####################

Introduction
*************

If you're not familiar with Docker, in short, it lets you create containers with
predefined images. Containers are something similar to a virtual machine. An
image is just the contents of the hard drive of that virtual machine.

It is very easy to share folders between the docker container and the host. That
way you can share your code with the container, build your code from the
container, and see the results in the host (so that you can test the result on
an emulator or copy it to a flashcart!).

The images are available here: https://hub.docker.com/r/skylyrac/blocksds

Setup
*****

First, install Docker: https://docs.docker.com/get-docker/ You may need to
reboot after the installation.

Then, pick your image:

- ``blocksds-dev``: For developers that want to improve BlocksDS.
- ``blocksds-slim``: For developers that just want to focus on NDS development.

``blocksds-slim`` is smaller, so it is the recommended image.

Both images have all the pre-built SDK, and you don't need to setup any
environment variable or anything.

Slim image
==========

Run the following command to get the image:

.. code:: sh

    docker image pull skylyrac/blocksds:slim-latest

To open a shell inside it, run:

.. code:: sh

    docker run --rm -v /path/to/your/code:/work -it --entrypoint bash \
        skylyrac/blocksds:slim-latest

The part of the command that shares a folder is ``-v /path/to/your/code:/work``.
This example will share the path ``/path/to/your/code`` from the host and show
it in ``/work`` inside the container.

To update the image, simply run the previous ``git image pull`` command again.

Development image
=================

Run the following command to get the image:

.. code:: sh

    docker image pull skylyrac/blocksds:dev-latest

To open a shell inside it, run:

.. code:: sh

    docker run --rm -v /path/to/your/code:/work -it --entrypoint bash \
        skylyrac/blocksds:dev-latest

The full git repository of BlocksDS is in ``/opt/blocksds/sdk/``.

Uninstall the images
********************

If you have installed Docker just for this, you can run the following command to
remove all docker images available in your PC:

.. code:: sh

    docker system prune -a

Build images locally
********************

From the ``docker`` folder of this repository run one of the two following
commands, depending on the image you want to build:

Development image:

.. code:: sh

    docker build --target blocksds-dev --tag blocksds:dev .

Slim image:

.. code:: sh

    docker build --target blocksds-slim --tag blocksds:dev .
