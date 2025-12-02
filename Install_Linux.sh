# W3SLAV was x

#some installed elements need sudo, so to improve user experience run with sudo so you don't have to enter a password 10 times
#this can be done with sudo bash Install_Linux.sh
python tools/Orca.py

export PATH=/opt/wonderful/bin:$PATH
export WONDERFUL_TOOLCHAIN=/opt/wonderful
ln -s /opt/wonderful/thirdparty/blocksds /opt/blocksds
export BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core
export BLOCKSDSEXT=/opt/wonderful/thirdparty/blocksds/external


echo "Install finished, but you may need to set the PATH for Wonderful Toolchain permanently"
