# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

# TODO: Currently it isn't possible to initialize the emulator in DSi mode
# without a NAND image. If it becomes possible, we should add tests to see that
# everything else can work without a NAND image.

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 20)

    for i in range(5):
        yield from repeat(JoypadState(a=True), 5)
        yield from repeat(0, 5)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

# DSi mode, NAND enabled, SD enabled, DLDI enabled
# ------------------------------------------------

user_options = {
    "melonds_console_mode": "dsi",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for i in range(5):
        bt.session_run_frames(session, 10)
        do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# DSi mode, NAND enabled, SD disabled, DLDI enabled
# -------------------------------------------------

user_options = {
    "melonds_console_mode": "dsi",
    "melonds_dsi_sdcard": "disabled",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for i in range(5):
        bt.session_run_frames(session, 10)
        do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# DSi mode, NAND enabled, SD disabled, DLDI disabled
# --------------------------------------------------

user_options = {
    "melonds_console_mode": "dsi",
    "melonds_dsi_sdcard": "disabled",
    "melonds_homebrew_sdcard": "disabled",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for i in range(5):
        bt.session_run_frames(session, 10)
        do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# DS mode, DLDI enabled
# ---------------------

user_options = {
    "melonds_console_mode": "ds",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for i in range(5):
        bt.session_run_frames(session, 10)
        do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# DS mode, DLDI disabled
# ----------------------

user_options = {
    "melonds_console_mode": "ds",
    "melonds_homebrew_sdcard": "disabled",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      user_dldi_root='test/dldi_root') as session:

    bt.session_run_frames(session, 20)
    do_screenshot(session)

    for i in range(5):
        bt.session_run_frames(session, 10)
        do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# ================================================

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
