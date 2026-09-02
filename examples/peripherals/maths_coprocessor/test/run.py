# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from libretro import UnformattedLogDriver

bt.delete_all_screenshots()

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver) as session:
    bt.session_run_frames(session, 20)
    res_1 = bt.save_screenshot(session, 'result.png', (0, 192, 256, 192+32))

    bt.assert_no_errors_in_logs(log_driver)

bt.compare_image_with_reference(res_1, 'reference.png')
