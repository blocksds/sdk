# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 30)

    for _ in range(4):
        yield from repeat(JoypadState(right=True), 5)
        yield from repeat(0, 5)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    for _ in range(4):
        yield from repeat(JoypadState(left=True), 5)
        yield from repeat(0, 5)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    for _ in range(4):
        yield from repeat(JoypadState(right=True), 5)
        yield from repeat(0, 5)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    for _ in range(4):
        yield from repeat(JoypadState(left=True), 5)
        yield from repeat(0, 5)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    for _ in range(4):
        yield from repeat(JoypadState(right=True), 5)
        yield from repeat(0, 5)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    for _ in range(4):
        yield from repeat(JoypadState(left=True), 5)
        yield from repeat(0, 5)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 30)

    for i in range(5):

        for _ in range(5):
            do_screenshot(session)
            bt.session_run_frames(session, 10)
        do_screenshot(session)

        bt.session_run_frames(session, 10)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
