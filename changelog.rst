##################
BlocksDS changelog
##################

Version 0.2 (2023-03-15)
========================

- Improve C++ support (now the C++ standard library it is actually usable).
- Improve C library support.
- Integrate agbabi as ndsabi (provides fast memcpy, coroutines, etc).
- Fix install target.

Version 0.1 (2023-03-14)
========================

First beta release of BlocksDS. Features:

- Supports libnds, maxmod, dswifi.
- Supports a lot of the standard C library.
- Very early support of the standard C++ library.
- Supports DLDI, DSi SD slot and NitroFAT (open source alternative of NitroFS)
  through Elm's FatFs.
- Documentation on how to migrate projects to BlocksDS.
- Docker image provided.

