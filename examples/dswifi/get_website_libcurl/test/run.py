# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

user_options = { "melonds_console_mode": "dsi" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:

    test_ok = False

    # Allow the test to run for up to 30 seconds
    for _ in range(30):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "HTTP/1.1 200 OK") and \
           bt.string_is_in_logs(log_driver, "Example Domain"):
               test_ok = True
               break

    assert test_ok

user_options = { "melonds_console_mode": "ds" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:

    test_ok = False

    # Allow the test to run for up to 30 seconds
    for _ in range(30):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "HTTP/1.1 200 OK") and \
           bt.string_is_in_logs(log_driver, "Example Domain"):
               test_ok = True
               break

    assert test_ok
