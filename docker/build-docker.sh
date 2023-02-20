#!/bin/bash

# This script is used to generate the Docker images and upload them to Docker
# Hub. It is only meant to be used by maintainers of the images.

set -e

docker build --target blocksds-dev --tag blocksds:dev .
docker tag blocksds:dev skylyrac/blocksds:dev-latest
docker push skylyrac/blocksds:dev-latest

docker build --target blocksds-slim --tag blocksds:slim .
docker tag blocksds:slim skylyrac/blocksds:slim-latest
docker push skylyrac/blocksds:slim-latest
