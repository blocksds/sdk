// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <nds/arm9/device_io.h>

#include "lfs.h"
#include "bd/lfs_rambd.h"

static lfs_t lfs;
static lfs_rambd_t bd_ctx;

#define BLOCK_SIZE      0x1000
#define NUM_OF_BLOCKS   28
#define FLASH_SIZE      (BLOCK_SIZE * NUM_OF_BLOCKS)

// The configuration of the filesystem is provided by this struct
const struct lfs_config cfg =
{
    .context = &bd_ctx,

    // Block device operations
    .read  = lfs_rambd_read,
    .prog  = lfs_rambd_prog,
    .erase = lfs_rambd_erase,
    .sync  = lfs_rambd_sync,

    // Block device configuration
    .read_size = 1,
    .prog_size = 8,
    .block_size = BLOCK_SIZE,
    .block_count = NUM_OF_BLOCKS,
    .cache_size = 512,
    .lookahead_size = 16,
    .block_cycles = 500,
};

struct lfs_rambd_config bd_cfg =
{
    // Minimum size of a read operation in bytes.
    .read_size = 1,

    // Minimum size of a program operation in bytes.
    .prog_size = 8,

    // Size of an erase operation in bytes.
    .erase_size = BLOCK_SIZE,

    // Number of erase blocks on the device.
    .erase_count = NUM_OF_BLOCKS,

    // Optional statically allocated buffer for the block device.
    .buffer = NULL
};

// -----------------------------------------------------------------------------

static int littlefs_to_errno(enum lfs_error error)
{
    switch (error)
    {
        case LFS_ERR_OK: // No error
            return 0;
        case LFS_ERR_IO: // Error during device operation
            return EIO;
        case LFS_ERR_CORRUPT: // Corrupted
            return EFAULT;
        case LFS_ERR_NOENT: // No directory entry
            return ENOENT;
        case LFS_ERR_EXIST: // Entry already exists
            return EEXIST;
        case LFS_ERR_NOTDIR: // Entry is not a dir
            return ENOTDIR;
        case LFS_ERR_ISDIR: // Entry is a dir
            return EISDIR;
        case LFS_ERR_NOTEMPTY: // Dir is not empty
            return ENOTEMPTY;
        case LFS_ERR_BADF: // Bad file number
            return EBADF;
        case LFS_ERR_FBIG: // File too large
            return ERANGE;
        case LFS_ERR_INVAL: // Invalid parameter
            return EINVAL;
        case LFS_ERR_NOSPC: // No space left on device
            return ENOMEM;
        case LFS_ERR_NOMEM: // No more memory available
            return ENOMEM;
        case LFS_ERR_NOATTR: // No data/attr available
            return ENODATA;
        case LFS_ERR_NAMETOOLONG: // File name too long
            return ERANGE;
    }

    return ENOMSG;
}

// -----------------------------------------------------------------------------

static bool littlefs_isdrive(const char *name)
{
    if (strcmp(name, "lfs") == 0)
        return true;

    return false;
}

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

static ssize_t littlefs_write(int fd, const void *ptr, size_t len)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    int ret = lfs_file_write(&lfs, file, ptr, len);
    if (ret < 0)
    {
        errno = littlefs_to_errno(ret);
        return -1;
    }

    return ret;
}

static ssize_t littlefs_read(int fd, void *ptr, size_t len)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    int ret = lfs_file_read(&lfs, file, ptr, len);
    if (ret < 0)
    {
        errno = littlefs_to_errno(ret);
        return -1;
    }

    return ret;
}

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

static int littlefs_stat(const char *file, struct stat *st)
{
    struct lfs_info info = { 0 };

    enum lfs_error err = lfs_stat(&lfs, file, &info);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    st->st_size = info.size;
    st->st_mode = (info.type == LFS_TYPE_DIR) ?
                   S_IFDIR : // Directory
                   S_IFREG;  // Regular file

    return 0;
}

static int littlefs_access(const char *path, int amode)
{
    (void)amode; // Ignore the mode, there is no read-only attribute in littlefs

    // Try to open as a file
    lfs_file_t file = { 0 };
    int err = lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
    if (err == LFS_ERR_OK)
    {
        lfs_file_close(&lfs, &file);
        return 0;
    }

    // Try to open as a directory

    lfs_dir_t dp = { 0 };
    int result = lfs_dir_open(&lfs, &dp, path);
    if (result == LFS_ERR_OK)
    {
        lfs_dir_close(&lfs, &dp);
        return 0;
    }

    errno = ENOENT;
    return -1;
}

static int littlefs_unlink(const char *name)
{
    // This is used for both files and empty directories
    enum lfs_error err = lfs_remove(&lfs, name);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}

static int littlefs_rename(const char *old, const char *new_)
{
    enum lfs_error err = lfs_rename(&lfs, old, new_);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}

static int littlefs_mkdir(const char *path, mode_t mode)
{
    (void)mode;

    enum lfs_error err = lfs_mkdir(&lfs, path);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}

static int littlefs_fsync(int fd)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    enum lfs_error err = lfs_file_sync(&lfs, file);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}

static int littlefs_ftruncate(int fd, off_t length)
{
    lfs_file_t *file = (lfs_file_t *)fd;

    enum lfs_error err = lfs_file_truncate(&lfs, file, length);
    if (err != LFS_ERR_OK)
    {
        errno = littlefs_to_errno(err);
        return -1;
    }

    return 0;
}

static int littlefs_isatty(int fd)
{
    (void)fd;

    // We could check if the file descriptor is valid, but that would force us
    // to check socket descriptors, nitrofs, etc. To make things easier, don't
    // check them. Instead of EBADF we will return ENOTTY always.
    errno = ENOTTY;
    return 0;
}

// -----------------------------------------------------------------------------

static int littlefs_chdir(const char *name)
{
    // littlefs doesn't suppor chdir()

    if (strcmp(name, "/") == 0)
        return 0;
    if (strcmp(name, "lfs:/") == 0)
        return 0;
    if (strcmp(name, ".") == 0)
        return 0;

    errno = EFAULT;
    return -1;
}

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

// -----------------------------------------------------------------------------

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

static void littlefs_rewinddir(DIR *dirp)
{
    lfs_dir_t *dp = dirp->dp;

    enum lfs_error err = lfs_dir_rewind(&lfs, dp);
    if (err != LFS_ERR_OK)
        errno = littlefs_to_errno(err);
}

static void littlefs_seekdir(DIR *dirp, long loc)
{
    lfs_dir_t *dp = dirp->dp;

    enum lfs_error err = lfs_dir_seek(&lfs, dp, loc);
    if (err != LFS_ERR_OK)
        errno = littlefs_to_errno(err);
}

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

// -----------------------------------------------------------------------------

static device_io_t littlefs_device =
{
    .isdrive = littlefs_isdrive,

    .open = littlefs_open,
    .close = littlefs_close,
    .write = littlefs_write,
    .read = littlefs_read,
    .lseek = littlefs_lseek,

    .fstat = NULL, // Not supported
    .stat = littlefs_stat,
    .lstat = littlefs_stat, // Symbolic links aren't supported

    .access = littlefs_access,

    .link = NULL, // Symbolic links aren't supported
    .unlink = littlefs_unlink,
    .rename = littlefs_rename,

    .mkdir = littlefs_mkdir,
    .chdir = littlefs_chdir,
    .rmdir = littlefs_unlink,
    .getcwd = littlefs_getcwd,
    .chdrive = NULL, // Only one drive supported

    .opendir = littlefs_opendir,
    .closedir = littlefs_closedir,
    .readdir = littlefs_readdir,
    .rewinddir = littlefs_rewinddir,
    .seekdir = littlefs_seekdir,
    .telldir = littlefs_telldir,

    .statvfs = NULL, // Not implemented
    .fstatvfs = NULL, // Not implemented
    .fsync = littlefs_fsync,
    .fdatasync = littlefs_fsync, // Aliased to fsync()

    .ftruncate = littlefs_ftruncate,
    .truncate = NULL, // Not supported by the filesystem

    // The filesystem doesn't support file modes
    .chmod = NULL,
    .fchmod = NULL,
    .fchmodat = NULL,

    // Not supported by the filesystem
    .utimes = NULL,
    .lutimes = NULL,
    .utime = NULL,

    .isatty = littlefs_isatty,

    // The filesystem doesn't support file owners
    .chown = NULL,
    .fchown = NULL,
    .fchownat = NULL,

    // Symbolic links aren't supported
    .symlink = NULL,
    .readlink = NULL,

    // Not implemented
    .get_attr = NULL,
    .set_attr = NULL,
    .get_short_name_for = NULL,
};

// -----------------------------------------------------------------------------

static int littlefs_device_index = -1;

int littlefs_init(void)
{
    printf("Initializing littlefs...\n");

    printf("Creating RAM block device...\n");

    int rc = lfs_rambd_create(&cfg, &bd_cfg);
    if (rc != 0)
    {
        printf("Error: lfs_rambd_create\n");
        return -1;
    }

    printf("Formatting and mounting...\n");

    // We can try to mount the filesystem before formatting it, but that only
    // works if we already have a valid filesystem image. This example starts
    // from an empty buffer, so we need to format it every time.
    //int err = lfs_mount(&lfs, &cfg);
    //if (err)
    {
        // Reformat if we can't mount the filesystem. This should only happen in
        // the first boot.
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    printf("Adding device to libnds...\n");

    littlefs_device_index = deviceIoAdd(&littlefs_device);
    if (littlefs_device_index < 0)
    {
        printf("Error: deviceIoAdd\n");
        return -1;
    }

    printf("littlefs is ready!\n");

    return 0;
}

void littlefs_end(void)
{
    printf("Deinitializing littlefs...\n");

    deviceIoRemove(littlefs_device_index);
    lfs_unmount(&lfs);
    lfs_rambd_destroy(&cfg);

    printf("littlefs is disabled!\n");
}
