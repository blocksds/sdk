#!/bin/bash

# For example, VERSION=v0.11.1

# Tag submodules
git submodule foreach 'git tag $VERSION-blocks ; git push origin $VERSION-blocks'

# Tag main repository
git tag $VERSION ; git push origin $VERSION
