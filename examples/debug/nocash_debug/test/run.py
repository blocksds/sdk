# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver) as session:
    bt.session_run_frames(session, 20)

    bt.string_is_in_logs(log_driver, "ARM9 10")
    bt.string_is_in_logs(log_driver, "ARM7 10")
