# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

def generate_input():
    # Wait for nds-hb-menu to boot
    yield from repeat(0, 20)

    yield from repeat(JoypadState(down=True), 5)
    yield from repeat(0, 5)
    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    # Wait for the test ROM to boot
    yield from repeat(0, 40)

    yield from repeat(JoypadState(a=True, b=True), 5)
    yield from repeat(0, 15)

    # Wait for nds-hb-menu to boot
    yield from repeat(0, 20)

    yield from repeat(JoypadState(down=True), 5)
    yield from repeat(0, 5)
    yield from repeat(JoypadState(a=True), 5)
    yield from repeat(0, 5)

    # Wait for the test ROM to boot
    yield from repeat(0, 40)

    yield from repeat(JoypadState(l=True, r=True, start=True, select=True), 5)
    yield from repeat(0, 15)

    # Wait for nds-hb-menu to boot
    yield from repeat(0, 20)

user_options = {
    "melonds_console_mode": "ds",
}

log_driver = UnformattedLogDriver()
with bt.session_start('test/hbmenu-0.11.0/boot.nds', generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)
    bt.session_run_frames(session, 60)
    do_screenshot(session)
    bt.session_run_frames(session, 40)
    do_screenshot(session)
    bt.session_run_frames(session, 60)
    do_screenshot(session)
    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

user_options = {
    "melonds_console_mode": "dsi",
}

log_driver = UnformattedLogDriver()
with bt.session_start('test/hbmenu-0.11.0/boot.nds', generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)
    bt.session_run_frames(session, 60)
    do_screenshot(session)
    bt.session_run_frames(session, 40)
    do_screenshot(session)
    bt.session_run_frames(session, 60)
    do_screenshot(session)
    bt.session_run_frames(session, 40)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
