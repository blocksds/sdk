#####################################
BlocksDS SDK installation from source
#####################################

This is the option you should use if you want to help develop BlocksDS, or if
you want to always use the cutting edge version of BlocksDS.

1. You need to install a cross compiler to build applications for NDS. You also
   need to build a few host tools, so you need a compiler for the host and the
   FreeImage library.

   If you're on Ubuntu, for example, run the following command:

   .. code:: bash

       sudo apt-get install -y --no-install-recommends \
           build-essential

2. You will also need to install the ARM toolchain of Wonderful Toolchains.
   Follow the instructions in this link but you skip step 5:
   https://wonderful.asie.pl/doc/general/getting-started/#linux

3. Now, install the ARM toolchain of Wonderful Toolchains:

   .. code:: bash

       export PATH=/opt/wonderful/bin:$PATH
       wf-pacman -Syu
       wf-pacman -S toolchain-gcc-arm-none-eabi

4. When you're done, remember to add the following to your ``PATH``:

   .. code:: bash

       export PATH=/opt/wonderful/toolchain/gcc-arm-none-eabi/bin/:$PATH

   You can avoid exporting ``PATH`` every time by adding it to your ``.bashrc``,
   ``.zshenv`` or similar.

5. Now, clone this repository:

   .. code:: bash

       git clone --recurse-submodules https://github.com/blocksds/sdk.git
       cd sdk

6. To build the SDK, run this from the root of this repository:

   .. code:: bash

       BLOCKSDS=$PWD make -j`nproc`

7. Now, you have some options. The first one is recommended, the others are for
   users that may have advanced needs.

   7.1. **Recommended**. Install it in your system. You won't need to set
        ``BLOCKSDS`` or ``BLOCKSDSEXT`` manually to use the SDK. Run:

        .. code:: bash

            sudo mkdir /opt/blocksds/ && sudo chown $USER:$USER /opt/blocksds
            mkdir /opt/blocksds/external
            make install

        This will install the libraries and tools to ``/opt/blocksds/core``.
        Third party libraries and tools are expected to be installed to
        ``/opt/blocksds/external``.

        ``BLOCKDS`` defaults to ``/opt/blocksds/core`` in all Makefiles, and
        ``BLOCKDSEXT`` defaults to ``/opt/blocksds/external``.

   7.2. Install it in your system in a custom path. You will need to set
        ``BLOCKSDS`` or ``BLOCKSDSEXT`` manually to use the SDK. Run:

        .. code:: bash

            BLOCKSDS=$PWD make INSTALLDIR=my/path -j`nproc`

        You can avoid exporting ``BLOCKSDS`` every time by adding it to your
        ``.bashrc``, ``.zshenv`` or similar.

   7.3. Use the libraries from this path. Make sure that the environment
        variable ``BLOCKSDS`` is always set to the right location when you want
        to use the SDK. The build system of the templates and examples will use
        this variable to locate the components of BlocksDS it and use them. Any
        other external library will need to be managed by you.

        You can avoid exporting ``BLOCKSDS`` every time by adding it to your
        ``.bashrc``, ``.zshenv`` or similar.
