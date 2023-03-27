/*
/ Copyright (C) 2017, ChaN, all right reserved.
/ Copyright (C) 2023, AntonioND, all right reserved.
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
*/

/*--------------------------------------------------------/
/  FAT image creator R0.02               (C)ChaN, 2017
/--------------------------------------------------------*/

#define DIR DIRff
#include "ff.h"
#undef DIR

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "diskio.h"


#if FF_MIN_SS != FF_MAX_SS
#error Sector size must be fixed at any value
#endif

#define MIN_FAT16	4086U	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526U	/* Minimum number of clusters for FAT32 */

#define BPB_NumFATs			16		/* Number of FAT copies (1) */
#define BPB_RootEntCnt		17		/* Number of root directory entries for FAT12/16 (2) */
#define BPB_TotSec16		19		/* Volume size [sector] (2) */
#define BPB_FATSz16			22		/* FAT size [sector] (2) */
#define BPB_TotSec32		32		/* Volume size [sector] (4) */
#define BPB_FATSz32			36		/* FAT size [sector] (4) */

/* External functions (ff.c) */
extern DWORD get_fat (FFOBJID* obj, DWORD);		/* Read an FAT item */
extern DWORD ld_dword (const BYTE* ptr);		/* Load a 4-byte little-endian word */
extern WORD ld_word (const BYTE* ptr);			/* Load a 2-byte little-endian word */
extern void st_word (BYTE* ptr, WORD val);		/* Store a 2-byte word in little-endian */
extern void st_dword (BYTE* ptr, DWORD val);	/* Store a 4-byte word in little-endian */


BYTE *RamDisk;		/* Poiter to the RAM disk */
DWORD RamDiskSize;	/* Size of RAM disk in unit of sector */

static int verbose = 0;

static FATFS FatFs;
static FIL DstFile;
static FILE *SrcFile;
static char SrcPath[512], DstPath[512];
static BYTE Buff[4096];
static UINT Dirs, Files;
static QWORD TotalFilesSize;

int treesize(void)
{
	DIR *pdir;
	int slen, dlen, rv = 0;

	slen = strlen(SrcPath);
	dlen = strlen(DstPath);

	pdir = opendir(SrcPath);		/* Open directory */
	if (pdir == NULL) {
		printf("Failed to open the source directory.\n");
		goto end;
	}

	struct dirent *pent;
	struct stat statbuf;

	while ((pent = readdir(pdir)) != NULL)
	{
		sprintf(&SrcPath[slen], "/%s", pent->d_name);
		sprintf(&DstPath[dlen], "/%s", pent->d_name);

		stat(SrcPath, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) {	/* The item is a directory */
			if (strcmp(pent->d_name, ".") && strcmp(pent->d_name, "..")) {
				if (verbose)
					printf("Dir:  %s\n", SrcPath);
				if (!treesize()) break;	/* Enter the directory */
			}
		} else {	/* The item is a file */
			if (verbose)
				printf("File: %s (%zu bytes)\n", SrcPath, statbuf.st_size);
			TotalFilesSize += statbuf.st_size;
		}
	}
	closedir(pdir);
	rv = 1;

end:
	SrcPath[slen] = 0;
	DstPath[dlen] = 0;
	return rv;
}

int maketree (void)
{
	DIR *pdir;
	int slen, dlen, rv = 0;
	DWORD br;
	UINT bw;

	slen = strlen(SrcPath);
	dlen = strlen(DstPath);

	pdir = opendir(SrcPath);		/* Open directory */
	if (pdir == NULL) {
		printf("Failed to open the source directory.\n");
		goto end;
	}

	struct dirent *pent;
	struct stat statbuf;

	while ((pent = readdir(pdir)) != NULL)
	{
		sprintf(&SrcPath[slen], "/%s", pent->d_name);
		sprintf(&DstPath[dlen], "/%s", pent->d_name);

		stat(SrcPath, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) {	/* The item is a directory */
			if (strcmp(pent->d_name, ".") && strcmp(pent->d_name, "..")) {
				if (verbose)
					printf("Creating: %s\n", SrcPath);
				if (f_mkdir(DstPath)) {	/* Create destination directory */
					printf("Failed to create directory.\n"); break;
				}
				if (!maketree()) break;	/* Enter the directory */
				Dirs++;
			}
		} else {	/* The item is a file */
			if (verbose)
				printf("Adding:   %s\n", SrcPath);
			if ((SrcFile = fopen(SrcPath, "rb")) == NULL) {	/* Open source file */
				printf("Failed to open source file.\n"); break;
			}
			if (f_open(&DstFile, DstPath, FA_CREATE_ALWAYS | FA_WRITE)) {	/* Create destination file */
				printf("Failed to create destination file.\n"); break;
			}
			do {	/* Copy source file to destination file */
				br = fread(Buff, 1, sizeof(Buff), SrcFile);
				if (br == 0) break;
				f_write(&DstFile, Buff, (UINT)br, &bw);
			} while (br == bw);
			fclose(SrcFile);
			f_close(&DstFile);
			if (br && br != bw) {
				printf("Failed to write file.\n"); break;
			}
			Files++;
		}
	}
	closedir(pdir);
	rv = 1;

end:
	SrcPath[slen] = 0;
	DstPath[dlen] = 0;
	return rv;
}


int main (int argc, char* argv[])
{
	UINT csz;
	FILE *fout;
	DWORD wb, szvol;
	DIRff dir;
	int ai = 1, truncation = 0;
	const char *outfile;


	/* Initialize parameters */
	while (argc >= 2 && *argv[ai] == '-') {
		if (!strcmp(argv[ai], "-t")) {
			truncation = 1;
			ai++;
			argc--;
		} else if (!strcmp(argv[ai], "-v")) {
			verbose = 1;
			ai++;
			argc--;
		} else {
			argc = 0;
		}
	}

	if (argc < 3) {
		printf("usage: mkfatimg [-t] [-v] <source node> <output image> <image size> [<cluster size>]\n"
				"    -t: Truncate unused area for read only volume.\n"
				"    -v: Verbose mode.\n"
				"    <source node>: Source node as root of output image\n"
				"    <output image>: FAT volume image file\n"
				"    <image size>: Size of output image in unit of sectors (0 = auto)\n"
				"    <cluster size>: Size of cluster in unit of byte (default:512)\n"
			);
		return 1;
	}
	strcpy(SrcPath, argv[ai++]);
	outfile = argv[ai++];
	RamDiskSize = atoi(argv[ai++]) * 2;
	csz = (argc >= 5) ? atoi(argv[ai++]) : 512;

	TotalFilesSize = 0;
	treesize();
	printf("Total size of files: %lu bytes\n", TotalFilesSize);

	/* If the user hasn't set the size, use an image size of 40% the total of
	 * the sizes of all files. */
	if (RamDiskSize == 0) {
		printf("Autocalculating size...\n");
		RamDiskSize = (TotalFilesSize * 140) / 100;
		RamDiskSize = (RamDiskSize / csz) + 1; /* Sectors */
	}

	/* Create an FAT volume (Supports only FAT/FAT32). This function can select
	 * FAT12, FAT16 or FAT32 automatically depending on the image size. */
	MKFS_PARM opt = {
		.fmt = FM_FAT | FM_FAT32 | FM_SFD,
		.n_fat = 1,
		.align = 0,
		.n_root = 1,
		.au_size = csz
	};
	if (f_mkfs("", &opt, Buff, sizeof Buff)) {
		printf("Failed to create FAT volume. Adjust volume size or cluster size.\n");
		return 2;
	}

	/* Copy source directory tree into the FAT volume */
	f_mount(&FatFs, "", 0);
	if (!maketree()) return 3;

	/* Right after f_mount() there is no filesystem type information */
	switch (FatFs.fs_type) {
	case FS_FAT12:
		printf("Using format FAT12");
		break;
	case FS_FAT16:
		printf("Using format FAT16");
		break;
	case FS_FAT32:
		printf("Using format FAT32");
		break;
	default:
		printf("Using format unknown (%d)\n", FatFs.fs_type);
		return 3;
	}

	if (!Files) { printf("No file in the source directory."); return 3; }
	szvol = ld_word(RamDisk + BPB_TotSec16);
	if (!szvol) szvol = ld_dword(RamDisk + BPB_TotSec32);

	if (truncation) {
		DWORD ent, nent;
		DWORD szf, szfp, edf, edfp;
		DWORD szd, szdp, edd, eddp;

		/* Truncate unused root directory entries */
		if (FatFs.fs_type != FS_FAT32) {
			printf("\nTruncating unused root directory area...");
			for (nent = ent = 0; ent < FatFs.n_rootdir; ent++) {	/* Get number of entries on the root dir */
				if (RamDisk[FatFs.dirbase * FF_MIN_SS + ent * 32]) nent = ent + 1;
			}
			szd = (nent + (FF_MIN_SS / 32 - 1)) / (FF_MIN_SS / 32);
			szdp = FatFs.n_rootdir / (FF_MIN_SS / 32);
			if (szd < szdp) {
				edd = FatFs.dirbase + szd;
				eddp = FatFs.database;
				memcpy(RamDisk + (edd * FF_MIN_SS), RamDisk + (eddp * FF_MIN_SS), (szvol - eddp) * FF_MIN_SS);
				szvol -= szdp - szd;
				FatFs.database -= szdp - szd;
				st_word(RamDisk + BPB_RootEntCnt, (WORD)(szd * (FF_MIN_SS / 32)));
			}
		}

		/* Truncate unused data area and FAT */
		printf("\nTruncating unused data area...");
		f_opendir(&dir, "");
		for (nent = ent = 2; ent < FatFs.n_fatent; ent++) {	/* Get number of used clusters */
			if (get_fat(&dir.obj, ent)) nent = ent + 1;
		}
		switch (FatFs.fs_type) {
		case FS_FAT12:
			szf = (nent * 3 / 2 + (nent & 1) + (FF_MIN_SS - 1)) / FF_MIN_SS;
			break;
		case FS_FAT16:
			szf = (nent * 2 + (FF_MIN_SS - 1)) / FF_MIN_SS;
			if (nent - 2 < MIN_FAT16) nent = 0;	/* Wrong cluster count for FAT16 */
			break;
		default:
			szf = (nent * 4 + (FF_MIN_SS - 1)) / FF_MIN_SS;
			if (nent - 2 < MIN_FAT32) nent = 0;	/* Wrong cluster count for FAT32 */
			break;
		}
		if (!nent) {
			printf("\nAnother FAT sub-type is requierd for truncation. Adjust volume size or cluster size.\n");
			return 3;
		}
		szfp = ld_word(RamDisk + BPB_FATSz16) * RamDisk[BPB_NumFATs];
		if (!szfp) szfp = ld_dword(RamDisk + BPB_FATSz32) * RamDisk[BPB_NumFATs];
		edf = FatFs.fatbase + szf;
		edfp = (FatFs.fs_type == FS_FAT32) ? FatFs.database : FatFs.dirbase;
		memcpy(RamDisk + (edf * FF_MIN_SS), RamDisk + (edfp * FF_MIN_SS), (szvol - edfp) * FF_MIN_SS);
		szvol -= (szfp - szf) + FatFs.csize * (FatFs.n_fatent - nent);
		if (FatFs.fs_type == FS_FAT32) {
			st_dword(RamDisk + BPB_FATSz32, szf);
		} else {
			st_word(RamDisk + BPB_FATSz16, (WORD)szf);
		}
		RamDisk[BPB_NumFATs] = 1;
		if (szvol < 0x10000) {
			st_word(RamDisk + BPB_TotSec16, (WORD)szvol);
			st_dword(RamDisk + BPB_TotSec32, 0);
		} else {
			st_word(RamDisk + BPB_TotSec16, 0);
			st_dword(RamDisk + BPB_TotSec32, szvol);
		}
	}

	/* Output the FAT volume to the file */
	printf("\nWriting output file...");
	fout = fopen(outfile, "wb");
	if (fout == NULL) {
		printf("Failed to create output file.\n");
		return 4;
	}
	szvol *= FF_MIN_SS;
	wb = fwrite(RamDisk, 1, szvol, fout);
	fclose(fout);
	if (szvol != wb) {
		remove(outfile);
		printf("Failed to write output file.\n");
		return 4;
	}

	printf("\n%u files and %u directories in the %uKiB of FAT volume.\n", Files, Dirs, szvol / 1024);

	return 0;
}
