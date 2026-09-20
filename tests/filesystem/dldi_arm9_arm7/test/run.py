# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input_arm7():
    yield from repeat(0, 30)

    yield from repeat(JoypadState(l=True), 5)
    yield from repeat(0, 5)

def generate_input_arm9():
    yield from repeat(0, 30)

    yield from repeat(JoypadState(r=True), 5)
    yield from repeat(0, 5)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png', (0, 192, 256, 384)))
    number = number + 1

user_options = {
    "melonds_console_mode": "ds",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input_arm7, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input_arm9, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
