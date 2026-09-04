# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 80)

    yield from repeat(JoypadState(a=True), 10)
    yield from repeat(0, 120)
    yield from repeat(JoypadState(a=True), 10)

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:
    bt.session_run_frames(session, 80)
    res_1 = bt.save_screenshot(session, 'result_1.png')

    bt.session_run_frames(session, 20)
    res_2 = bt.save_screenshot(session, 'result_2.png')

    bt.session_run_frames(session, 120)
    res_3 = bt.save_screenshot(session, 'result_3.png')

    bt.assert_no_errors_in_logs(log_driver)

bt.compare_image_with_reference(res_1, 'reference_1.png')
bt.compare_image_with_reference(res_2, 'reference_2.png')
bt.compare_image_with_reference(res_3, 'reference_3.png')
