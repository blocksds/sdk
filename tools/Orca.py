#idk, license or something
# W3SLAV was x

import os
import platform
import argparse
import subprocess
#urllib is only for downloading Wonderful Toolchain builds
from urllib.request import urlretrieve


#add MSYS command mode check
MSYS_MODE = False

#error checking
cwd = os.getcwd()
def run(cmd):
    if (MYS_MODE==True):
        subprocess.run("C:\msys64\usr\bin\bash.exe -lc "+cmd, shell=True, check=True)
    else:
        subprocess.run(cmd, shell=True, check=True)

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
    if "MSYSTEM" not in os.environ:
        print("Not running inside MSYS2. Installing MSYS2 + WF-PACMAN + BLOCKS")

        #this will need a good way to be set to latest always VV
        urlretrieve("https://github.com/msys2/msys2-installer/releases/download/2025-06-22/msys2-x86_64-20250622.exe", "msys2_installer.exe")
        #silent install based on: https://silentinstallhq.com/msys2-silent-install-how-to-guide/
        run("msys2_installer.exe install --root C:\MSYS2 --confirm-command")

        #install wf-pacman
        urlretrieve("https://wonderful.asie.pl/bootstrap/wf-bootstrap-windows-x86_64.exe", "wf-installer.exe")
        run("wf-installer.exe /VERYSILENT /NORESTART")

        MSYS_MODE = True

    else:
        pacman_path = shutil.which("wf-pacman")
        if not pacman_path:
            urlretrieve("https://wonderful.asie.pl/bootstrap/wf-bootstrap-windows-x86_64.exe", "wf-installer.exe")
            run("wf-installer.exe /VERYSILENT /NORESTART")

            MSYS_MODE = True

if (is_portable==False):
#pull from https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz
    if (platform.system() == "Linux"):
        urlretrieve("https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz", "wf-bootstrap-x86_64.tar.gz")

        #following the tutorial here:
        run('mkdir /opt/wonderful')
        run('chown -R $USER:$USER /opt/wonderful')
        run('cd /opt/wonderful/')
        run('tar xzvf '+cwd+'/wf-bootstrap-x86_64.tar.gz -C /opt/wonderful')
        os.environ["PATH"] = "/opt/wonderful/bin:" + os.environ["PATH"]
        os.environ["WONDERFUL_TOOLCHAIN"] = "/opt/wonderful"
        run('cd /opt/wonderful/bin')
        run('wf-pacman -Syu --noconfirm')
        run('wf-pacman -S toolchain-gcc-arm-none-eabi --noconfirm')

        print('finished installing Wonderful Toolchain\n beginning BlocksDS install')

# THIS WILL RUN ON ALL PLATFORMS
    run('wf-pacman -Syu wf-tools --noconfirm')
    run('wf-config repo enable blocksds --noconfirm')
    run('wf-pacman -Syu --noconfirm')
    run('wf-pacman -S blocksds-toolchain')
    # this will install docs as well V
    if (args.minimal==False):
        run('wf-pacman -S blocksds-docs')
        run('wf-pacman -S toolchain-llvm-teak-llvm')
        run('wf-pacman -S blocksds-nflib blocksds-nitroengine')
        run('ln -s /opt/wonderful/thirdparty/blocksds /opt/blocksds')









#LINUX/WSL ONLY VVVVV
if (is_portable==True):
    if (platform.system() == "Linux"):
        #build to "portable file" called portable.blocks
        run("mkdir portable.blocks")
        run("cd portable.blocks")
        if os.path.exists("/opt/wonderful"):
            run('cp -r /opt/wonderful '+cwd+'/portable.blocks')
            print("finished portable Wonderful Toolchain packing\nmoving on to Blocks...")
        else:
            print("building a portable copy requires having wf-pacman installed please install it by running this program without the '--portable' flag")
            exit()
        #build blocks to dir
        run('BLOCKSDS=$PWD make INSTALLDIR=portable.blocks -j`nproc`')


print("\nDone!\n")


