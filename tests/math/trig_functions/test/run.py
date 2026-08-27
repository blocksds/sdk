# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import os

from PIL import Image, ImageChops
from itertools import repeat

from libretro import JoypadState, UnformattedLogDriver
from blocksds_testing import blocksds_test_session, blocksds_test_assert_no_errors_in_logs

print('TEST START')

def generate_input():
    yield from repeat(0, 500)
    yield from repeat(JoypadState(a=True), 50)
    yield from repeat(0, 500)
    yield from repeat(JoypadState(a=True), 50)
    yield from repeat(0, 500)

files = os.listdir(os.getcwd())
rom_path = None
for f in files:
    if f.endswith('.nds'):
        rom_path = f
        break

assert rom_path is not None

print(f'NDS ROM: {rom_path}')

os.makedirs('build', exist_ok=True)

ref_1 = Image.open('test/reference_1.png')
ref_2 = Image.open('test/reference_2.png')
ref_3 = Image.open('test/reference_3.png')

log_driver = UnformattedLogDriver()
with blocksds_test_session(rom_path, generate_input, log_driver) as session:
    for i in range(500):
        session.run()

    res_1 = Image.frombytes('RGBA', (256, 384), session.video.screenshot().data.obj)
    res_1.save('build/result_1.png', "PNG")

    for i in range(500):
        session.run()

    res_2 = Image.frombytes('RGBA', (256, 384), session.video.screenshot().data.obj)
    res_2.save('build/result_2.png', "PNG")

    for i in range(500):
        session.run()

    res_3 = Image.frombytes('RGBA', (256, 384), session.video.screenshot().data.obj)
    res_3.save('build/result_3.png', "PNG")

    logs = [r.message for r in log_driver.records]
    assert logs is not None
    assert len(logs) > 0
    blocksds_test_assert_no_errors_in_logs(logs)

diff = ImageChops.difference(ref_1, res_1)
if diff.getbbox():
    print('ERROR: Reference 1 mismatch')
    assert False

diff = ImageChops.difference(ref_2, res_2)
if diff.getbbox():
    print('ERROR: Reference 2 mismatch')
    assert False

diff = ImageChops.difference(ref_3, res_3)
if diff.getbbox():
    print('ERROR: Reference 3 mismatch')
    assert False

print('TEST END')
