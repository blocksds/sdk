# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt
import itertools

from math import sin
from itertools import repeat
from libretro import JoypadState, UnformattedLogDriver

bt.delete_all_screenshots()

def generate_input():
    yield from repeat(0, 60)

    yield from repeat(JoypadState(r=True), 5)
    yield from repeat(0, 5)

def generate_sine_wave() -> Iterator[int]:
    for i in itertools.count():
            yield int(sin(i * 20) * 10000)

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png'))
    number = number + 1

user_options = {
    "melonds_mic_input_active": "always",
    "melonds_mic_input": "microphone"
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), generate_input, log_driver, user_options,
                      microphone_gen=generate_sine_wave) as session:
    bt.session_run_frames(session, 55)
    do_screenshot(session)
    bt.session_run_frames(session, 8)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
