#!/bin/bash

# Run this from the root of the repository.
#
#    NEW_VERSION=v0.11.0 bash scripts/do-new-release-tasks.sh

set -e

echo ""
echo "[***] Tagging all repositories"
echo ""

VERSION=$NEW_VERSION bash scripts/tag-version-all-repos.sh

echo ""
echo "[***] Run documentation workflow from here:"
echo "[***] https://github.com/blocksds/docs/actions/workflows/build.yml"
echo ""
echo "[***] Update blocksds/packages repository manually"
echo "[***] Wait for the pipeline to finish, then press ENTER"
echo ""

read -n 1 -s

echo ""
echo "[***] Building Docker images"
echo ""

cd docker

VERSION=latest BRANCH=$NEW_VERSION bash build-docker.sh
VERSION=$NEW_VERSION BRANCH=$NEW_VERSION bash build-docker.sh
