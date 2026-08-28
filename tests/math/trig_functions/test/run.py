# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

def generate_input():
    yield from repeat(0, 30)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 25)

    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 25)

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 30)
    res_1 = bt.save_screenshot(session, 'result_1.png')

    bt.session_run_frames(session, 30)
    res_2 = bt.save_screenshot(session, 'result_2.png')

    bt.session_run_frames(session, 30)
    res_3 = bt.save_screenshot(session, 'result_3.png')

    bt.assert_no_errors_in_logs(log_driver)

bt.compare_image_with_reference(res_1, 'reference_1.png')
bt.compare_image_with_reference(res_2, 'reference_2.png')
bt.compare_image_with_reference(res_3, 'reference_3.png')
