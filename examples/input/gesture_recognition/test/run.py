# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, Pointer, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 180)

    # Start N

    for y in range(0, 100, 5):
        yield from repeat(Pointer(*bt.touch(30, 130 - y), True), 1)

    yield from repeat(0, 5)

    for i in range(0, 100, 5):
        yield from repeat(Pointer(*bt.touch(30 + i, 30 + i), True), 1)

    yield from repeat(0, 5)

    for y in range(0, 100, 5):
        yield from repeat(Pointer(*bt.touch(130, 130 - y), True), 1)

    yield from repeat(0, 20)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 25)

    # Start X

    for i in range(0, 100, 5):
        yield from repeat(Pointer(*bt.touch(30 + i, 30 + i), True), 1)

    yield from repeat(0, 5)

    for i in range(0, 100, 5):
        yield from repeat(Pointer(*bt.touch(130 - i, 30 + i), True), 1)

    yield from repeat(0, 20)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 15)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 180)
    do_screenshot(session)

    bt.session_run_frames(session, 80)
    do_screenshot(session)

    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.session_run_frames(session, 50)
    do_screenshot(session)

    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')

