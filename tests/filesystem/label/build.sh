#!/bin/bash

make

cd test
bash generate_images.sh
cd ..
