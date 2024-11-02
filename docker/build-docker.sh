#!/bin/bash

# This script is used to generate the Docker images and upload them to Docker
# Hub. It is only meant to be used by maintainers of the images.
#
# It is possible to overwrite the version by invoking the script like this:
#
#     VERSION=v0.1 sh build-docker.sh
#
# It is also possible to specify a branch or tag with, for example,
# "BRANCH=v0.11.0". If VERSION is set to "latest" branch will default to
# "master" unless it is set by the user to a different value.

set -e

if [ -z $VERSION ]; then
    VERSION="latest"
fi

if [ -z $BRANCH ]; then
    if [ "$VERSION" = "latest" ]; then
        BRANCH="master"
    else
        BRANCH="$VERSION"
    fi
fi

if [ -z $PLATFORM ]; then
    PLATFORM="linux/amd64,linux/arm64"
fi

echo "VERSION = $VERSION"
echo "BRANCH = $BRANCH"
echo "PLATFORM = $PLATFORM"

docker build --target blocksds-dev --tag blocksds:dev \
             --build-arg BRANCH=$BRANCH --platform $PLATFORM .
docker tag blocksds:dev skylyrac/blocksds:dev-$VERSION

docker build --target blocksds-slim --tag blocksds:slim \
             --build-arg BRANCH=$BRANCH --platform $PLATFORM .
docker tag blocksds:slim skylyrac/blocksds:slim-$VERSION

docker push skylyrac/blocksds:dev-$VERSION
docker push skylyrac/blocksds:slim-$VERSION
