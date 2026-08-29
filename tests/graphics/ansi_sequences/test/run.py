# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 15)

    while True:
        yield from repeat(JoypadState(a=True), 2)
        yield from repeat(0, 3)

res = []

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:

    bt.session_run_frames(session, 20)

    # clear console

    res.append(bt.save_screenshot(session, 'result_1.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_2.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_3.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_4.png'))
    bt.session_run_frames(session, 10)

    # clear line

    res.append(bt.save_screenshot(session, 'result_5.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_6.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_7.png'))
    bt.session_run_frames(session, 10)
    res.append(bt.save_screenshot(session, 'result_8.png'))

    # move cursor

    bt.session_run_frames(session, 80)
    res.append(bt.save_screenshot(session, 'result_9.png'))

    # set cursor

    bt.session_run_frames(session, 60)
    res.append(bt.save_screenshot(session, 'result_10.png'))

    # colors

    bt.session_run_frames(session, 5)
    res.append(bt.save_screenshot(session, 'result_11.png'))

    bt.assert_no_errors_in_logs(log_driver)

for i in range(11):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
