# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

user_options = { "melonds_console_mode": "dsi" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)

    assert bt.string_is_in_logs(log_driver, "[TEST] ARM9 PASSED")
    assert bt.string_is_in_logs(log_driver, "[TEST] ARM7 PASSED")

user_options = { "melonds_console_mode": "ds" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)

    assert bt.string_is_in_logs(log_driver, "[TEST] ARM9 PASSED")
    assert bt.string_is_in_logs(log_driver, "[TEST] ARM7 PASSED")
