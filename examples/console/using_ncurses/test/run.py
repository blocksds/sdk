# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, Pointer, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 40)

    for _ in range(2):
        yield from repeat(JoypadState(l=True), 10)
        yield from repeat(0, 10)

    # Main menu

    for _ in range(4):
        yield from repeat(JoypadState(r=True), 10)
        yield from repeat(0, 10)

    # First example

    yield from repeat(Pointer(*bt.key_to_touch('1'), True), 10)
    yield from repeat(0, 10)

    for _ in range(4):
        yield from repeat(JoypadState(l=True), 10)
        yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.key_to_touch('space'), True), 10)
    yield from repeat(0, 10)

    # Second example

    yield from repeat(Pointer(*bt.key_to_touch('2'), True), 10)
    yield from repeat(0, 10)

    for _ in range(4):
        yield from repeat(JoypadState(r=True), 10)
        yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.key_to_touch('space'), True), 10)
    yield from repeat(0, 10)

    # Third example

    yield from repeat(Pointer(*bt.key_to_touch('3'), True), 10)
    yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.key_to_touch('return'), True), 10)
    yield from repeat(0, 10)

    for _ in range(4):
        yield from repeat(JoypadState(l=True), 10)
        yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.key_to_touch('q'), True), 10)
    yield from repeat(0, 10)

    # Fourth example

    yield from repeat(Pointer(*bt.key_to_touch('4'), True), 10)
    yield from repeat(0, 10)

    yield from repeat(JoypadState(select=True), 10)
    yield from repeat(0, 10)

    for i in range (10, 100):
        yield from repeat(Pointer(*bt.touch(i * 2, i), True), 1)
    yield from repeat(0, 10)
    yield from repeat(JoypadState(r=True), 10)
    yield from repeat(0, 10)

    for i in range (10, 100):
        yield from repeat(Pointer(*bt.touch(200 - i * 2, i), True), 1)
    yield from repeat(0, 10)
    yield from repeat(JoypadState(r=True), 10)
    yield from repeat(0, 10)

    for i in range (10, 100):
        yield from repeat(Pointer(*bt.touch(200 - i * 2, i), True), 1)
    yield from repeat(0, 10)
    yield from repeat(JoypadState(r=True), 10)
    yield from repeat(0, 10)

    for i in range (10, 100):
        yield from repeat(Pointer(*bt.touch(200 - i * 2, 160 - i), True), 1)
    yield from repeat(0, 10)
    yield from repeat(JoypadState(r=True), 10)
    yield from repeat(0, 10)

    for i in range (10, 100):
        yield from repeat(Pointer(*bt.touch(i * 2, 160 - i), True), 1)
    yield from repeat(0, 10)
    yield from repeat(JoypadState(r=True), 10)
    yield from repeat(0, 10)

    yield from repeat(JoypadState(select=True), 10)
    yield from repeat(0, 10)

    yield from repeat(Pointer(*bt.key_to_touch('q'), True), 10)
    yield from repeat(0, 10)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 80)

    # Main menu

    for _ in range(5):
        do_screenshot(session)
        bt.session_run_frames(session, 20)

    # First example

    for _ in range(5):
        do_screenshot(session)
        bt.session_run_frames(session, 20)

    # Second example

    bt.session_run_frames(session, 20)

    for _ in range(5):
        do_screenshot(session)
        bt.session_run_frames(session, 20)

    # Third example

    bt.session_run_frames(session, 20)
    do_screenshot(session)
    bt.session_run_frames(session, 20)

    for _ in range(5):
        do_screenshot(session)
        bt.session_run_frames(session, 20)

    # Fourth example

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for _ in range(5):
        bt.session_run_frames(session, 100)
        do_screenshot(session)
        bt.session_run_frames(session, 20)
        do_screenshot(session)

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
