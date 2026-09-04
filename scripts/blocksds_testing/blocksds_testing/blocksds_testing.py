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

def session_start(game, input_gen=None, log_driver=None, user_options=None):
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

    default_options = {
        'melonds_jit_enable': 'disabled',
        'melonds_homebrew_sync_sdcard_to_host': 'enabled',
        'melonds_console_mode' : 'ds',
        'melonds_dsi_nand_path' : 'nand.bin',
        'melonds_firmware_dsi_path' : 'dsfirmware.bin',
        'melonds_show_cursor': 'disabled',
    }

    if user_options is not None:
        # User options override the default options
        options = default_options | user_options
    else:
        options = default_options

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

def save_screenshot(session, name, box=None):
    os.makedirs('build', exist_ok=True)
    path = os.path.join('build', name)

    # Convert to RGB to ensure that we compare images with the same format
    img = Image.frombytes('RGBA', (256, 384), session.video.screenshot().data.obj).convert('RGB')

    if box is not None:
        img = img.crop(box)

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

def touch(x, y):
    '''Use normal DS screen coordinates. x = 0 .. 255, y = 0 .. 191'''

    # Some information about how this works:
    # https://github.com/libretro/RetroArch/blob/0eb4107a35737c806e39ba5d9070e1d5417c4c84/libretro-common/include/libretro.h#L262-L305

    x = int((((x / 255) - 0.5) * 2) * 0x7FFF)
    y = int((y / 191) * 0x7FFF)
    return x, y

def key_to_coordinates(key, mapping='lower'):

    lowercase_mapping = {
        'fold' : (0, 0),
        #'nokey' : (16, 0),
        '1' : (24, 0),
        '2' : (40, 0),
        '3' : (56, 0),
        '4' : (72, 0),
        '5' : (88, 0),
        '6' : (104, 0),
        '7' : (120, 0),
        '8' : (136, 0),
        '9' : (152, 0),
        '0' : (168, 0),
        '-' : (184, 0),
        '=' : (200, 0),
        'backspace' : (216, 0),

        'tab' : (0, 16),
        'q' : (32, 16),
        'w' : (48, 16),
        'e' : (64, 16),
        'r' : (80, 16),
        't' : (96, 16),
        'y' : (112, 16),
        'u' : (128, 16),
        'i' : (144, 16),
        'o' : (160, 16),
        'p' : (176, 16),
        '[' : (192, 16),
        ']' : (208, 16),
        '\\' : (224, 16),
        '`' : (240, 16),

        'caps' : (0, 32),
        'a' : (40, 32),
        's' : (56, 32),
        'd' : (72, 32),
        'f' : (88, 32),
        'g' : (104, 32),
        'h' : (120, 32),
        'j' : (136, 32),
        'k' : (152, 32),
        'l' : (168, 32),
        ';' : (184, 32),
        "'" : (200, 32),
        'return' : (216, 32),

        'shift' : (0, 48),
        'z' : (48, 48),
        'x' : (64, 48),
        'c' : (80, 48),
        'v' : (96, 48),
        'b' : (112, 48),
        'n' : (128, 48),
        'm' : (144, 48),
        ',' : (160, 48),
        '.' : (176, 48),
        '/' : (192, 48),
        #'nokey' : (208, 48),
        'up' : (224, 48),
        #'nokey' : (240, 48),

        'ctrl' : (0, 64),
        'alt' : (40, 64),
        'space' : (72, 64),
        'menu' : (168, 64),
        'left' : (208, 64),
        'down' : (224, 64),
        'right' : (240, 64),
    }

    uppercase_mapping = {
        'fold' : (0, 0),
        #'nokey' : (16, 0),
        '!' : (24, 0),
        '@' : (40, 0),
        '#' : (56, 0),
        '$' : (72, 0),
        '%' : (88, 0),
        '^' : (104, 0),
        '&' : (120, 0),
        '*' : (136, 0),
        '(' : (152, 0),
        ')' : (168, 0),
        '_' : (184, 0),
        '+' : (200, 0),
        'backspace' : (216, 0),

        'tab' : (0, 16),
        'Q' : (32, 16),
        'W' : (48, 16),
        'E' : (64, 16),
        'R' : (80, 16),
        'T' : (96, 16),
        'Y' : (112, 16),
        'U' : (128, 16),
        'I' : (144, 16),
        'O' : (160, 16),
        'P' : (176, 16),
        '{' : (192, 16),
        '}' : (208, 16),
        '|' : (224, 16),
        '~' : (240, 16),

        'caps' : (0, 32),
        'A' : (40, 32),
        'S' : (56, 32),
        'D' : (72, 32),
        'F' : (88, 32),
        'G' : (104, 32),
        'H' : (120, 32),
        'J' : (136, 32),
        'K' : (152, 32),
        'L' : (168, 32),
        ':' : (184, 32),
        '"' : (200, 32),
        'return' : (216, 32),

        'shift' : (0, 48),
        'Z' : (48, 48),
        'X' : (64, 48),
        'C' : (80, 48),
        'V' : (96, 48),
        'B' : (112, 48),
        'N' : (128, 48),
        'M' : (144, 48),
        '<' : (160, 48),
        '>' : (176, 48),
        '?' : (192, 48),
        #'nokey' : (208, 48),
        'up' : (224, 48),
        #'nokey' : (240, 48),

        'ctrl' : (0, 64),
        'alt' : (40, 64),
        'space' : (72, 64),
        'menu' : (168, 64),
        'left' : (208, 64),
        'down' : (224, 64),
        'right' : (240, 64),
    }

    if mapping == 'lower':
        x, y = lowercase_mapping[key]
    elif mapping == 'upper':
        x, y = uppercase_mapping[key]
    else:
        return None

    # Base Y coordinate of the keyboard
    offset_y = 112

    # Return the center of the key
    return x + 8, y + 8 + offset_y

def key_to_touch(key, mapping='lower'):
    return touch(*key_to_coordinates(key, mapping))
