#idk, license or something
# W3SLAV was x


#this tool only works on x86 linux platforms for the time being (because Wonderful Toolchain doesn't support windows)
import os

#urllib is only for downloading Wonderful Toolchain builds
from urllib.request import urlretrieve

#this is for simple "portable" option (no path exports and all files sent to single dir)
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("--portable",action="store_true", help="build in portable mode")
args = parser.parse_args()


print("OrcaBlocks Installer v1.0\n    -W3SLAV")

is_portable = False

if (args.portable==True):
    print("using portable mode")
    print("THIS MODE IS NOT RECOMMENDED FOR NORMAL USE")
    is_portable = True


#pull from https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz
urlretrieve("https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz", "wf-bootstrap-x86_64.tar.gz")

#get current path for later
#cwd = blocks sdk dir
cwd = os.getcwd()

if (is_portable==False):
    #following the tutorial here:
    os.system('mkdir /opt/wonderful')
    os.system('chown -R $USER:$USER /opt/wonderful')
    os.system('cd /opt/wonderful/')
    os.system('tar xzvf '+cwd+'/wf-bootstrap-x86_64.tar.gz -C /opt/wonderful')
    os.environ["PATH"] = "/opt/wonderful/bin:" + os.environ["PATH"]
    os.environ["WONDERFUL_TOOLCHAIN"] = "/opt/wonderful"
    os.system('cd /opt/wonderful/bin')
    os.system('wf-pacman -Syu --noconfirm')
    os.system('wf-pacman -S toolchain-gcc-arm-none-eabi --noconfirm')

    print('finished installing Wonderful Toolchain\n beginning BlocksDS install')


    #start installing blocks
    os.system('sudo mkdir /opt/blocksds/')
    os.system('sudo chown $USER:$USER /opt/blocksds')
    os.system('mkdir /opt/blocksds/external')
    os.system('make install')


if (is_portable==True):
    #build to "portable file" called portable.blocks

    os.system('mkdir portable.blocks')
    os.system('cd portable.blocks')
    os.system('tar xzvf '+cwd+'/wf-bootstrap-x86_64.tar.gz -C '+cwd+'/portable.blocks')
    #VVV this needs Wonderful toolchain to make a few changes to how it searches for the config file, I will make an issue on github for this
    os.system('cd '+cwd+'/portable.blocks/bin/ && ./wf-pacman -Syu')
    os.system('cd '+cwd+'/portable.blocks/bin/ && ./wf-pacman -S toolchain-gcc-arm-none-eabi')
    print("finished portable Wonderful Toolchain packing\nmoving on to Blocks...")

    #build blocks to dir
    os.system('BLOCKSDS=$PWD make INSTALLDIR=portable.blocks -j`nproc`')


print("\nDone!\n")


