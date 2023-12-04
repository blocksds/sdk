#!/bin/bash

# Run this from the root of the repository.
#
#    NEW_VERSION=v0.11.0 bash scripts/do-new-release-tasks.sh

VERSION=$NEW_VERSION bash scripts/tag-version-all-repos.sh

cd docker

VERSION=latest BRANCH=$NEW_VERSION bash build-docker.sh
VERSION=$NEW_VERSION BRANCH=$NEW_VERSION bash build-docker.sh
