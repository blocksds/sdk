# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import os
import shutil

from PIL import Image, ImageChops

from libretro import Session, TempDirPathDriver
from libretro.drivers import (ArrayAudioDriver, ArrayVideoDriver,
                              DictOptionDriver, IterableInputDriver,
                              StandardContentDriver)

def session_start(game, input_gen=None, log_driver=None):
    blobs_path = os.environ['BLOCKSDS_TESTING_BLOBS']

    assert os.path.isabs(blobs_path)

    core_path = os.path.join(blobs_path, 'melondsds_libretro.so')

    path_driver = TempDirPathDriver(core_path, 'libretro')
    testdir = path_driver.root_dir
    system_dir = path_driver.system_dir
    playlist_dir = path_driver.playlist_dir
    save_dir = path_driver.save_dir
    save_directory = save_dir
    savestate_directory = os.path.join(testdir, b'states')
    core_system_dir = os.path.join(system_dir, b'melonDS DS')
    core_save_dir = os.path.join(save_dir, b'melonDS DS')
    wfcsettings_path = os.path.join(core_system_dir, b'wfcsettings.bin')
    dldi_sd_card_path = os.path.join(core_save_dir, b'dldi_sd_card.bin')
    dldi_sd_card_sync_path = os.path.join(core_save_dir, b'dldi_sd_card')

    print('[*] Test dir:', testdir)

    os.makedirs(core_system_dir, exist_ok=True)
    os.makedirs(core_save_dir, exist_ok=True)
    os.makedirs(dldi_sd_card_sync_path, exist_ok=True)

    options = {
        'melonds_jit_enable': 'disabled',
        'melonds_homebrew_sync_sdcard_to_host': 'enabled',
        'melonds_console_mode' : 'ds',
        'melonds_dsi_nand_path' : 'nand.bin',
        'melonds_firmware_dsi_path' : 'dsfirmware.bin',
    }

    if not os.path.exists(os.path.join(system_dir, b'nand.bin')):
        os.symlink(os.path.join(blobs_path, 'nand.bin'), os.path.join(system_dir, b'nand.bin'))
        os.symlink(os.path.join(blobs_path, 'dsfirmware.bin'), os.path.join(system_dir, b'dsfirmware.bin'))
        os.symlink(os.path.join(blobs_path, 'bios7i.bin'), os.path.join(system_dir, b'dsi_bios7.bin'))
        os.symlink(os.path.join(blobs_path, 'bios9i.bin'), os.path.join(system_dir, b'dsi_bios9.bin'))
        os.symlink(os.path.join(blobs_path, 'bios7.bin'), os.path.join(system_dir, b'bios7.bin'))
        os.symlink(os.path.join(blobs_path, 'bios9.bin'), os.path.join(system_dir, b'bios9.bin'))

    return Session(
        core=core_path,
        game=game,
        content=StandardContentDriver(),
        audio=ArrayAudioDriver(),
        input=IterableInputDriver(input_gen),
        video=ArrayVideoDriver(),
        options=DictOptionDriver(variables=options),
        path=path_driver,
        log=log_driver,
    )

def session_run_frames(session, frames):
    for i in range(frames):
        session.run()

def assert_no_errors_in_logs(log_driver):
    logs = [r.message for r in log_driver.records]

    assert logs is not None
    assert len(logs) > 0
    assert not any('data abort' in log for log in logs)
    assert not any('Failed to load' in log for log in logs)

def string_is_in_logs(log_driver, string):
    logs = [r.message for r in log_driver.records]

    assert logs is not None
    assert any(string in log for log in logs)

def find_rom(dir_path=None):
    if dir_path is None:
        dir_path = os.getcwd()

    files = os.listdir(dir_path)
    rom_path = None
    for f in files:
        if f.lower().endswith('.nds'):
            rom_path = f
            break

    assert rom_path is not None
    print(f'[*] NDS ROM: {rom_path}')
    return rom_path

def delete_all_screenshots():
    if not os.path.exists('build'):
        return

    files = os.listdir('build')
    rom_path = None
    for f in files:
        if f.lower().endswith('.png'):
            path = os.path.join('build', f)
            os.remove(path)

def save_screenshot(session, name):
    os.makedirs('build', exist_ok=True)
    path = os.path.join('build', name)

    # Convert to RGB to ensure that we compare images with the same format
    img = Image.frombytes('RGBA', (256, 384), session.video.screenshot().data.obj).convert('RGB')
    img.save(path, 'PNG')

    return img

def compare_image_with_reference(img, name):
    path = os.path.join('test', name)
    # Convert to RGB to ensure that we compare images with the same format
    ref = Image.open(path).convert('RGB')

    # This function doesn't behave properly unless the format of both images is
    # the same.
    diff = ImageChops.difference(img, ref)
    if diff.getbbox() is not None:
        print(f'ERROR: {path} comparison failed')
        assert False
