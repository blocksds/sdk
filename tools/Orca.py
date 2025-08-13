#idk, license or something
# W3SLAV was x

import os
import platform
import argparse
#urllib is only for downloading Wonderful Toolchain builds
from urllib.request import urlretrieve


cwd = os.getcwd()


parser = argparse.ArgumentParser()
parser.add_argument("--portable",action="store_true", help="build in portable mode (LINUX)")
parser.add_argument("--minimal",action="store_true", help="minimal install")
args = parser.parse_args()


print("OrcaBlocks Installer v1.0\n    -W3SLAV")

is_portable = False
if (args.portable==True):
    print("using portable mode")
    print("THIS MODE IS NOT RECOMMENDED FOR NORMAL USE")
    is_portable = True




#use msys if windows
if (platform.system() == "Windows"):
    env = os.environ
    return (
        "MSYSTEM" in env and
        env["MSYSTEM"] in ("MSYS", "MINGW32", "MINGW64", "UCRT64", "CLANG64")
    )
    if is_msys2():
        pacman_path = shutil.which("wf-pacman")
        if not pacman_path:
            exit()
    else:
        print("Not running inside MSYS2. Please Use MSYS2")
        exit()

if (is_portable==False):
#pull from https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz
    if (platform.system() == "Linux"):
        urlretrieve("https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz", "wf-bootstrap-x86_64.tar.gz")

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

# THIS WILL RUN ON ALL PLATFORMS
    os.system('wf-pacman -Syu wf-tools --noconfirm')
    os.system('wf-config repo enable blocksds --noconfirm')
    os.system('wf-pacman -Syu --noconfirm')
    os.system('wf-pacman -S blocksds-toolchain')
    # this will install docs as well V
    if (args.minimal==False):
        os.system('wf-pacman -S blocksds-docs')
        os.system('wf-pacman -S toolchain-llvm-teak-llvm')
        os.system('wf-pacman -S blocksds-nflib blocksds-nitroengine')
    os.system('ln -s /opt/wonderful/thirdparty/blocksds /opt/blocksds')









#LINUX/WSL ONLY VVVVV
if (is_portable==True):
    if (platform.system() == "Linux"):
        #build to "portable file" called portable.blocks

        os.system('mkdir portable.blocks')
        os.system('cd portable.blocks')
        if os.path.exists("/opt/wonderful"):
            os.system('cp -r /opt/wonderful '+cwd+'/portable.blocks')
            print("finished portable Wonderful Toolchain packing\nmoving on to Blocks...")
        else:
            print("building a portable copy requires having wf-pacman installed please install it by running this program without the '--portable' flag")
            exit()
        #build blocks to dir
        os.system('BLOCKSDS=$PWD make INSTALLDIR=portable.blocks -j`nproc`')


print("\nDone!\n")


