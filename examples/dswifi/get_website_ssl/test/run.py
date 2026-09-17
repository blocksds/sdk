# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

select_enabled = False

def generate_input_ok():
    global select_enabled

    while select_enabled == False:
        yield from repeat(0, 1)

    print("[TEST] Selecting website: valid certificate")

    yield from repeat(JoypadState(x=True), 10)
    yield from repeat(0, 60*10)

user_options = { "melonds_console_mode": "dsi" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input_ok, log_driver, user_options) as session:
    select_enabled = False
    test_ok = False

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Select option"):
            select_enabled = True
            print("[TEST] Reached menu to select website")
            break

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "HTTP/1.1 200 OK") and \
           bt.string_is_in_logs(log_driver, "Example Domain"):
               test_ok = True
               break

    assert test_ok

def generate_input_fail():
    global select_enabled

    while select_enabled == False:
        yield from repeat(0, 1)

    print("[TEST] Selecting website: invalid certificate")

    yield from repeat(JoypadState(b=True), 10)
    yield from repeat(0, 60*10)


user_options = { "melonds_console_mode": "ds" }

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input_fail, log_driver, user_options) as session:
    select_enabled = False
    test_ok = False

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Select option"):
            select_enabled = True
            print("[TEST] Reached menu to select website")
            break

    # Allow the test to run for up to 60 seconds
    for _ in range(60):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "CA or signature check failed"):
            test_ok = True
            break

    assert test_ok
