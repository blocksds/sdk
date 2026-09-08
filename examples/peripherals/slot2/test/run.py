# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import blocksds_testing as bt

from itertools import repeat
from libretro import UnformattedLogDriver

bt.delete_all_screenshots()

res = []
number = 1

def do_screenshot(session):
    global number

    res.append(bt.save_screenshot(session, f'result_{number}.png', (0, 192, 256, 384)))
    number = number + 1

# Test RAM expansion pak

user_options = {
    "melonds_console_mode": "ds",
    "melonds_slot2_device": "expansion-pak" # Official Opera Slot-2 RAM
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

# Test rumble pak

user_options = {
    "melonds_console_mode": "ds",
    "melonds_slot2_device": "rumble-pak" # DS Rumble Pak
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options) as session:
    bt.session_run_frames(session, 20)
    do_screenshot(session)
    # TODO: Test rumble pak better?

    bt.assert_no_errors_in_logs(log_driver)

# Test solar sensor

def generate_sensor_readings():
    yield from repeat(0, 20)
    yield from repeat(32000, 10)  # Direct sunlight
    yield from repeat(400, 10)  # Sunset
    yield from repeat(0, 20)

user_options = {
    "melonds_console_mode": "ds",
    "melonds_slot2_device": "solar1", # GBA Solar sensor
    "melonds_solar_sensor_host_sensor": "enabled",
}

log_driver = UnformattedLogDriver()
with bt.session_start(bt.find_rom(), None, log_driver, user_options,
                      sensor=generate_sensor_readings) as session:
    bt.session_run_frames(session, 25)
    do_screenshot(session)
    bt.session_run_frames(session, 10)
    do_screenshot(session)
    bt.session_run_frames(session, 10)
    do_screenshot(session)

    bt.assert_no_errors_in_logs(log_driver)

for i in range(len(res)):
    bt.compare_image_with_reference(res[i], f'reference_{i + 1}.png')
