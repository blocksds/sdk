#!/bin/bash

# This script is used to generate the Docker images and upload them to Docker
# Hub. It is only meant to be used by maintainers of the images.
#
# It is possible to overwrite the version by invoking the script like this:
#
#     VERSION=v0.1 sh build-docker.sh

set -e

if [ -z $VERSION ]; then
    VERSION="latest"
fi

docker build --target blocksds-dev --tag blocksds:dev .
docker tag blocksds:dev skylyrac/blocksds:dev-$VERSION
docker push skylyrac/blocksds:dev-$VERSION

docker build --target blocksds-slim --tag blocksds:slim .
docker tag blocksds:slim skylyrac/blocksds:slim-$VERSION
docker push skylyrac/blocksds:slim-$VERSION
