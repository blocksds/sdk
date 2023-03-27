/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "integer.h"


/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


DSTATUS disk_status (BYTE pdrv);
DSTATUS disk_initialize (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
DWORD get_fattime(void);


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (used at read/write cfg.) */
#define GET_SECTOR_COUNT	1	/* Get media size (used in f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (used at variable sector size, _MAX_SS > _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (used in f_mkfs()) */
#define CTRL_ERASE_SECTOR	4	/* Force erased a block of sectors (used at _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_FORMAT			5	/* Create physical format on the media */
#define CTRL_POWER_IDLE		6	/* Put the device idle state */
#define CTRL_POWER_OFF		7	/* Put the device off state */
#define CTRL_LOCK			8	/* Lock media removal */
#define CTRL_UNLOCK			9	/* Unlock media removal */
#define CTRL_EJECT			10	/* Eject media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		50	/* Get card type */
#define MMC_GET_CSD			51	/* Get CSD */
#define MMC_GET_CID			52	/* Get CID */
#define MMC_GET_OCR			53	/* Get OCR */
#define MMC_GET_SDSTAT		54	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			60	/* Get F/W revision */
#define ATA_GET_MODEL		61	/* Get model name */
#define ATA_GET_SN			62	/* Get serial number */

#ifdef __cplusplus
}
#endif

#endif
