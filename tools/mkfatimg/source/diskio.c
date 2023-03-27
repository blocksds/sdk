/*------------------------------------------------------------------------/
/  Low level disk I/O module SKELETON for FatFs                           /
/-------------------------------------------------------------------------/
/
/ Copyright (C) 2019, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* RAM disk control module for Win32              (C)ChaN, 2014          */
/*-----------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "diskio.h"
#include "ff.h"


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


extern BYTE *RamDisk;		/* Poiter to the active RAM disk (main.c) */
extern DWORD RamDiskSize;	/* Size of RAM disk in unit of sector */


/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv		/* Physical drive nmuber */
)
{
	if (pdrv) return STA_NOINIT;

	if (!RamDisk) {
		RamDisk = calloc(1, RamDiskSize * FF_MIN_SS);
	}

	return RamDisk ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0) */
)
{
	if (pdrv) return STA_NOINIT;

	return RamDisk ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to read */
)
{
	if (pdrv || !RamDisk) return RES_NOTRDY;
	if (sector >= RamDiskSize) return RES_PARERR;

	memcpy(buff, RamDisk + sector * FF_MIN_SS, count * FF_MIN_SS);

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write */
)
{
	if (pdrv || !RamDisk) return RES_NOTRDY;
	if (sector >= RamDiskSize) return RES_PARERR;

	memcpy(RamDisk + sector * FF_MIN_SS, buff, count * FF_MIN_SS);

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void* buff		/* Buffer to send/receive data block */
)
{
	DRESULT dr;


	dr = RES_ERROR;
	if (!pdrv && RamDisk) {
		switch (ctrl) {
		case CTRL_SYNC:
			dr = RES_OK;
			break;

		case GET_SECTOR_COUNT:
			*(DWORD*)buff = RamDiskSize;
			dr = RES_OK;
			break;

		case GET_BLOCK_SIZE:
			*(DWORD*)buff = 1;
			dr = RES_OK;
			break;
		}
	}
	return dr;
}


