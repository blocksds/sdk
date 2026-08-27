# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

import os
import shutil

from libretro import Session, TempDirPathDriver
from libretro.drivers import (ArrayAudioDriver, ArrayVideoDriver,
                              DictOptionDriver, IterableInputDriver,
                              StandardContentDriver)

def blocksds_test_session(game, input_gen=None, log=None) -> Session:
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
        log=log,
    )

def blocksds_test_assert_no_errors_in_logs(logs):
    assert not any('data abort' in log for log in logs)
    assert not any('Failed to load' in log for log in logs)
