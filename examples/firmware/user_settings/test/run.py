# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

bt.delete_all_screenshots()

user_options = {
    "melonds_firmware_alarm_hour": "0",
    "melonds_firmware_alarm_minute": "0",
    "melonds_firmware_enable_alarm": "false",

    "melonds_firmware_birth_month": "3", # First release of BlocksDS
    "melonds_firmware_birth_day": "13",

    "melonds_firmware_favorite_color": "10", # Light Blue
    "melonds_firmware_language": "English",

    # "existing_username", "guess_username" or "melonDS DS"
    "melonds_firmware_username": "melonDS DS",

    # The user message can't be configured here, so we can't check it as part of
    # this test.
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)
    res_1 = bt.save_screenshot(session, 'result_1.png', (0, 192, 256, 192+40))
    res_2 = bt.save_screenshot(session, 'result_2.png', (0, 192+88, 256, 192+120))

    bt.assert_no_errors_in_logs(log_driver)

bt.compare_image_with_reference(res_1, 'reference_1.png')
bt.compare_image_with_reference(res_2, 'reference_2.png')
