# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

# TODO: The ARM7 test currently fails in melonDS because it doesn't support
# emulating the undefined instruction handler.

rom_path = bt.find_rom()
bt.delete_all_screenshots()

def generate_input_arm9():
    yield from repeat(0, 20)
    yield from repeat(JoypadState(a=True), 10)
    yield from repeat(0, 20)

#def generate_input_arm7():
#    yield from repeat(0, 20)
#    yield from repeat(JoypadState(b=True), 10)
#    yield from repeat(0, 20)

log_driver = UnformattedLogDriver()
with bt.session_start(rom_path, generate_input_arm9, log_driver) as session:
    bt.session_run_frames(session, 40)
    res_1 = bt.save_screenshot(session, 'result_1.png', (0, 192, 256, 192+24))
    #bt.assert_no_errors_in_logs(log_driver) # A data abort is required

#log_driver = UnformattedLogDriver()
#with bt.session_start(rom_path, generate_input_arm7, log_driver) as session:
#    bt.session_run_frames(session, 40)
#    res_2 = bt.save_screenshot(session, 'result_2.png', (0, 192, 256, 192+24))
#    #bt.assert_no_errors_in_logs(log_driver) # A data abort is required

bt.compare_image_with_reference(res_1, 'reference_1.png')
#bt.compare_image_with_reference(res_2, 'reference_2.png')
