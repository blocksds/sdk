# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 30)

    for i in range(6):
        yield from repeat(JoypadState(a=True), 5)
        yield from repeat(0, 25)

res = []

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver) as session:

    for i in range(7):
        bt.session_run_frames(session, 30)
        img = bt.save_screenshot(session, f'result_{i + 1}.png')
        res.append(img)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(7):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
