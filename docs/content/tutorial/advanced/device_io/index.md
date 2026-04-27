---
title: Device I/O
weight: 5
---

## 1. Introduction

Applications using libnds have access to the following filesystems through
standard library functions implemented in picolibc (`fopen()`, `rename()`, etc).
The following drives are available in libnds:

- `fat:/`: The device acessible with the DLDI driver. This is normally the
  device that contains the ROM (a slot-1 or slot-2 flashcart). This is normally
  useful in DS, and not so much in DSi.
- `sd:/`: The SD card slot of a DSi console (not available in DS consoles).
- `nand:/` and `nand2:/`: Two of the NAND partitions of a DSi console. `nand:/`
  contains most of the system files while `nand2:/` contains the photos taken by
  the camera application.

However, the developer may want to implement their own filesystem and integrate
it with the standard I/O functions of picolibc.

For example, users of a game may be able to create filesystem images with
additional levels. The game could pack every level in an image using a
filesystem format like [littlefs](https://github.com/littlefs-project/littlefs)
and it could open them at runtime to load the level.

You could simply use the functions exposed in the header of littlefs (for
example, `lfs_file_open()`), but they are different from the standard C
functions and you will need custom code to use them. However, if you create a
custom device for libnds, you can use the same code for that device or any other
device.

BlocksDS comes with an example of how to add basic littlefs support to libnds:
[`examplesfilesystem/device_io_basic`](https://codeberg.org/blocksds/sdk/src/branch/master/examples/filesystem/device_io_basic).
You can use it as reference when you're implementing a new filesystem.

You should also check the manual pages of the C system calls as you are
implementing them. Most callbacks are supposed to work exactly like the system
call they implement, so you need to make them return similar error codes, for
example.

{{< callout type="tip" >}}
You don't need to check for NULL pointers in any of the callbacks. The global
handling code in [libnds](https://codeberg.org/blocksds/libnds/src/commit/59c453f62425542cd175cf23f0212d414e3dfbd1/source/arm9/libc/syscalls_filesystem.c)
has some basic checks that have to be repeated in all filesystems. Please,
report any missing checks that you think should be in the global handlers.
{{< /callout >}}

## 2. File operations

The most basic operation you can do with a filesystem is to open a file, read
data from it, and close it. Let's see how to create a device that can do that.

The first thing you need to do is to create a `device_io_t` struct. This struct
consists on a lot of function pointers that you need to provide. For example:

```c
#include <nds/arm9/device_io.h>

static device_io_t littlefs_device =
{
    .isdrive = littlefs_isdrive,

    .open = littlefs_open,
    .close = littlefs_close,
    .write = littlefs_write,
    .read = littlefs_read,
    .lseek = littlefs_lseek,
};
```

The `issdrive` callback is used by libnds to determine if a specific path
belongs to this filesystem or not.

The callbacks `open`, `close`, `write`, `read` and `lseek` correspond to the
syscalls with the same names. The callbacks are called internally by libc
functions like `fopen()`, `fclose()`, `fwrite()`, `fread()` and `fseek()`.

{{< callout type="tip" >}}
You are allowed to leave any callback as `NULL` if your filesystem doesn't
support that operation. For example, you can set `write` to `NULL` if your
filesystem is read-only. When libnds sees it, it will set `errno` a pre-defined
error code and return.
{{< /callout >}}

You can register the new device like this:

```c
int littlefs_device_index = deviceIoAdd(&littlefs_device);
if (littlefs_device_index < 0)
{
    // Handle error
}
```

The function may fail if there are too many registered devices. If it succeedes,
it returns an index that identifies the device. You can use it to unregister the
device later:

```c
deviceIoRemove(littlefs_device_index);
```

### isdrive

It receives a drive name and it returns `true` if that drive name corresponds to
this device or not. For example, if you try to open file `"lfs:/dir/file.txt"`
the `isdrive()` callback will be called. The argument will be `"lfs"` (not
`"lfs:"` or `"lfs:/"`), so the code should look like this:

```c
static bool littlefs_isdrive(const char *name)
{
    if (strcmp(name, "lfs") == 0)
        return true;

    return false;
}
```

You're free to use any name you want (the length is limited to names up to
`DEVICE_IO_MAX_DRIVE_NAME_LENGTH` characters long!). You aren't limited to one
name. If you want, you could make all `test0` to `test9` drive names return
`true`. Your filesystem implementation must keep track of the drive used to open
each file if needed.

### open

The implementation of the `open` callback must allocate any memory required to
keep a file open, and it needs to return a unique file descriptor. There file
descriptor is a 32-bit integer that must have the 4 top bits and the 2 bottom
bits set to zero. Those bits are reserved by libnds and they are used to
identify which device is associated to the file descriptor.

{{< callout type="tip" >}}
The reserved bits were chosen so that you can use `malloc()` to allocate your
own custom struct in main RAM,simply cast the address to an integer, and use
that as file descriptor. The top 4 bits of the NDS address space aren't used, so
they are free to be used as a tag. The bottom 2 bits can also be reserved
because the word size of a DS is 4 bytes, and addresses are normally aligned to
4 bytes. Note that negative numbers are invalid file descriptors, so the top bit
can't be used as part of the tag.
{{< /callout >}}

The file descriptors passed from libnds to device I/O callbacks have the
reserved bits set to zero, so you don't need to clear them yourself.

The callback `open` must also check the flags that define the mode in which the
file is opened (for reading, for writing, for appending...). You will need some
translation code between the flags used by your own filesystem and the ones used
by the syscall `open()`. For more information, run `man open` in the terminal or
check [this link](https://www.man7.org/linux/man-pages/man2/open.2.html).

```c
static int littlefs_open(const char *path, int flags, mode_t mode)
{
    lfs_file_t *file = calloc(1, sizeof(lfs_file_t));
    if (file == NULL)
        return -1;

    int lfs_flags = 0;

    switch (flags & (O_RDONLY | O_WRONLY | O_RDWR))
    {
        case O_RDONLY:
            lfs_flags = LFS_O_RDONLY;
            break;
        case O_WRONLY:
            lfs_flags = LFS_O_WRONLY;
            break;
        case O_RDWR:
            lfs_flags = LFS_O_RDWR;
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    if (flags & O_CREAT)
        lfs_flags |= LFS_O_CREAT;

    if (flags & O_APPEND)
        lfs_flags |= LFS_O_APPEND;

    if (flags & O_TRUNC)
        lfs_flags |= LFS_O_TRUNC;

    if (flags & O_EXCL)
        lfs_flags |= LFS_O_EXCL;

    enum lfs_error err = lfs_file_open(&lfs, file, path, lfs_flags);
    if (err != LFS_ERR_OK)
    {
        free(file);
        errno = littlefs_to_errno(err);
        return -1;
    }

    return (intptr_t)file;
}
```

### close

This callback must free all memory used by the file descriptor and ensure that
the file is closed:

```c
static int littlefs_close(int fd)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    enum lfs_error err = lfs_file_close(&lfs, file);

    free(file);

    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}
```

### Other callbacks

Other callbacks (like `read`, `write` or `lseek`) are generally a thin wrapper
that calls the corresponding filesystem function. For example:

```c
static off_t littlefs_lseek(int fd, off_t pos, int whence)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    int ret = lfs_file_seek(&lfs, file, pos, whence);
    if (ret < 0)
    {
        errno = littlefs_to_errno(ret);
        return -1;
    }

    return ret;
}
```

## 3. Directory operations

Directories work in a very similar way as files, but with fewer restrictions
because directory iterators don't use a file descriptor, so there aren't any
reserved bits to take care of.

The list of callbacks you need to implement are the following:

```c
static device_io_t littlefs_device =
{
    // ...

    .opendir = littlefs_opendir,
    .closedir = littlefs_closedir,
    .readdir = littlefs_readdir,
    .rewinddir = littlefs_rewinddir,
    .seekdir = littlefs_seekdir,
    .telldir = littlefs_telldir,

    // ...
};
```

### opendir

The callback `opendir` already receives a valid `DIR *`, and it is in charge of
allocating data to keep track of the state of the directory:

```c
static void *littlefs_opendir(const char *name, DIR *dirp)
{
    lfs_dir_t *dp = calloc(1, sizeof(lfs_dir_t));
    if (dp == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }

    enum lfs_error err = lfs_dir_open(&lfs, dp, name);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        free(dp);
        return NULL;
    }

    return dp;
}
```

The callback must return a pointer to the allocated data. This pointer is saved
by libnds in the `DIR`, in field `dp`. This means you can access your custom
data in all other callbacks by using `dirp->dp`.

### closedir

This callback is in charge of closing the directory and freeing the custom data
allocated inthe  `opendir` callback.

```c
static int littlefs_closedir(DIR *dirp)
{
    lfs_dir_t *dp = dirp->dp;

    enum lfs_error err = lfs_dir_close(&lfs, dp);

    free(dirp->dp);

    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}
```

### readdir

This callback needs to fill the data in `dirp->dirent` and return a pointer to
this `dirent` struct. The struct is cleared by libnds before calling the
callback, so you don't need to do it here:

```c
static struct dirent *littlefs_readdir(DIR *dirp)
{
    lfs_dir_t *dp = dirp->dp;

    struct lfs_info info = { 0 };

    int ret = lfs_dir_read(&lfs, dp, &info);
    if (ret < 0)
    {
        errno = littlefs_to_errno(ret);
        return NULL;
    }

    struct dirent *ent = &(dirp->dirent);
    ent->d_off = ret;

    strncpy(ent->d_name, info.name, sizeof(ent->d_name));
    ent->d_name[sizeof(ent->d_name) - 1] = '\0';

    if (info.type == LFS_TYPE_DIR)
        ent->d_type = DT_DIR; // Directory
    else
        ent->d_type = DT_REG; // Regular file

    return ent;
}
```

### rewinddir, seekdir, telldir

They are normally thin wrappers on top of the filesystem code. For example:

```c
static long littlefs_telldir(DIR *dirp)
{
    lfs_dir_t *dp = dirp->dp;

    int ret = lfs_dir_tell(&lfs, dp);
    if (ret < 0)
    {
        errno = littlefs_to_errno(ret);
        return -1;
    }

    return ret;
}
```

## 4. Current working directory

The current working directory is the directory that is used as starting point
when opening files and directories with relative paths. Not every filesystem
implementation supports changing it, but you should implement callbacks `chdir`
and `getcwd` to maximize compatibility. `chdrive` is only required if you want
to support multiple drives.

```c
static device_io_t littlefs_device =
{
    // ...

    .chdir = littlefs_chdir,
    .getcwd = littlefs_getcwd,
    .chdrive = NULL, // Only one drive supported

    // ...
};
```

### chdir, getcwd

When `chdir` gets called you need to change the global state of the filesystem.
The path received in the callback is the path that the syscall `chdir()` has
received without the drive name. A trivial implementation for a filesystem that
doesn't support `chdir()` should still accept things like `"/"` and `"."` to
maximize compatibility:

```c
static int littlefs_chdir(const char *name)
{
    // littlefs doesn't suppor chdir()

    if (strcmp(name, "/") == 0)
        return 0;
    if (strcmp(name, ".") == 0)
        return 0;

    errno = EFAULT;
    return -1;
}
```

In this case, the corresponding implementation of the `getcwd` callback would
look like this:

```c
static int littlefs_getcwd(char *buf, size_t size)
{
    // littlefs doesn't suppor chdir()

    const char *root = "lfs:/";

    if (size <= strlen(root))
    {
        errno = ENOMEM;
        return -1;
    }

    strcpy(buf, root);

    return 0;
}
```

### chdrive

This isn't a real standard C system call, but the implementation of `chdir()` in
libnds will call it when it detects it needs to change the current drive.

If your filesystem supports more than one drive (or if you want to have multiple
alias for the same drive) you need to implement callback `chdrive`. If you only
support one drive name, you don't need to implement this callback.

For example, you could load multiple littlefs images and switch between them by
having multiple drive names (from `"lfs0:"` to `"lfs9:"`):

```c
static int current_littlefs_drive = -1;

static int littlefs_chdrive(char *name)
{
    if (strlen(name) > 4)
    {
        errno = EINVAL;
        return -1;
    }

    // Only compare the first 3 characters
    if (strncmp(name, "lfs", strlen("lfs")) != 0)
    {
        errno = EINVAL;
        return -1;
    }

    char c = name[3] - '0';
    if ((c < 0) || (c > 9))
    {
        errno = EINVAL;
        return -1;
    }

    // Remember the drive number selected right now
    current_littlefs_drive = c;
    return 0;
}
```

You would need to modify the `isdrive` callback a bit for this to work:

```c
static bool littlefs_isdrive(const char *name)
{
    // Only drives with the format "lfsX" are allowed
    if (strlen(name) != 4)
        return false;

    // Only compare the first 3 characters
    if (strncmp(name, "lfs", strlen("lfs")) != 0)
        return false;

    // Check that the 4th character is a valid number
    char c = name[3] - '0';
    if ((c < 0) || (c > 9))
        return false;

    return true;
}
```
