# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

bt.delete_all_screenshots()

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

user_options = {
    "melonds_console_mode": "dsi",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:
    test_ok = False

    # Allow the test to run for up to 2 seconds
    for _ in range(2):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Test passed"):
            test_ok = True
            break

    assert test_ok

user_options = {
    "melonds_console_mode": "ds",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options,
                      user_dldi_root='test/dldi_root',
                      user_dsi_sd_image='test/dsi_sd_root.img') as session:
    test_ok = False

    # Allow the test to run for up to 2 seconds
    for _ in range(2):
        bt.session_run_frames(session, 60)

        if bt.string_is_in_logs(log_driver, "[TEST] Test passed"):
            test_ok = True
            break

    assert test_ok

# Don't check the screenshots, only the text output. The screenshots are saved
# to help debug problems with the test
