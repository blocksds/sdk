# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 20)

    yield from repeat(JoypadState(a=True, b=True), 20)
    yield from repeat(0, 50)

    yield from repeat(JoypadState(select=True), 200)
    yield from repeat(0, 50)

res = []

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 20)
    res.append(bt.save_screenshot(session, 'result_1.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 5)
    res.append(bt.save_screenshot(session, 'result_2.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_3.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_4.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_5.png', (0, 192, 348, 384)))

    bt.session_run_frames(session, 40)
    res.append(bt.save_screenshot(session, 'result_6.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 20)
    res.append(bt.save_screenshot(session, 'result_7.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 40)
    res.append(bt.save_screenshot(session, 'result_8.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 70)
    res.append(bt.save_screenshot(session, 'result_9.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 30)
    res.append(bt.save_screenshot(session, 'result_10.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 30)
    res.append(bt.save_screenshot(session, 'result_11.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_12.png', (0, 192, 348, 384)))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_13.png', (0, 192, 348, 384)))

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
