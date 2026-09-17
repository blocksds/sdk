# Exit to loader test

This test is used to verify that ROMs built with BlocksDS can return to the
loader that has loaded them. The loader needs to support the exit-to-loader
protocol described [here](https://blocksds.skylyrac.net/docs/internal/exit_to_loader/).

This test uses [The Nintendo DS Homebrew Menu v0.11.0](https://github.com/devkitPro/nds-hb-menu/releases/tag/v0.11.0)
because it's one of the loaders that supports this protocol.
