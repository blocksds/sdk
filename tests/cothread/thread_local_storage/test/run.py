# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver) as session:
    test_ok = False

    # Allow the test to run for up to 3 seconds
    for _ in range(3):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Test passed"):
            test_ok = True
            break

        if bt.string_is_in_logs(log_driver, "[TEST] Test failed"):
            break

    assert test_ok
