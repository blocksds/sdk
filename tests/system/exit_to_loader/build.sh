#!/bin/bash

make
cp *.nds test/dldi_root/
cp *.nds test/dsi_sd_root/

cd test
bash generate_images.sh
cd ..
