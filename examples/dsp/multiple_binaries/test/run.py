# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, Pointer, UnformattedLogDriver
from libretro.drivers.input.iterable import PortState

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 20)

    yield from repeat(JoypadState(a=True, up=True), 10)

    yield from repeat(0, 10)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png', (0, 192, 256, 384)))
    number = number + 1

user_options = { "melonds_console_mode": "dsi" } # ds, dsi, auto

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)
    do_screenshot(session)
    bt.session_run_frames(session, 20)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')

