# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, Pointer, UnformattedLogDriver

bt.delete_all_screenshots()

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png', (0, 192, 256, 384)))
    number = number + 1

def generate_input():
    yield from repeat(0, 10)

    # Move the models so that the cone is over the sphere
    yield from repeat(JoypadState(right=True), 40)
    yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.touch(80, 40), True), 8)
    yield from repeat(0, 2)

    yield from repeat(Pointer(*bt.touch(175, 115), True), 8)
    yield from repeat(0, 2)

    yield from repeat(Pointer(*bt.touch(165, 115), True), 8)
    yield from repeat(0, 2)

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 10)
    do_screenshot(session)
    bt.session_run_frames(session, 50)
    do_screenshot(session)

    bt.session_run_frames(session, 5)
    do_screenshot(session)
    bt.session_run_frames(session, 10)
    do_screenshot(session)
    bt.session_run_frames(session, 10)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
