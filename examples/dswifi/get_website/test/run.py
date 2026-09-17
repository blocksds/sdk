# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

select_enabled = False

def generate_input():
    global select_enabled

    yield from repeat(0, 60)
    yield from repeat(JoypadState(a=True), 10)

    while select_enabled == False:
        yield from repeat(0, 1)

    print("[TEST] Selecting IPv4+IPv6")

    yield from repeat(JoypadState(y=True), 10)
    yield from repeat(0, 60*10)

user_options = { "melonds_console_mode": "dsi" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options) as session:
    select_enabled = False
    test_ok = False

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Select IP type"):
            select_enabled = True
            print("[TEST] Reached menu to select IP type")
            break

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "HTTP/1.1 200 OK") and \
           bt.string_is_in_logs(log_driver, "Example Domain"):
               test_ok = True
               break

    assert test_ok

user_options = { "melonds_console_mode": "ds" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options) as session:
    select_enabled = False
    test_ok = False

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Select IP type"):
            select_enabled = True
            print("[TEST] Reached menu to select IP type")
            break

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "HTTP/1.1 200 OK") and \
           bt.string_is_in_logs(log_driver, "Example Domain"):
               test_ok = True
               break

    assert test_ok
