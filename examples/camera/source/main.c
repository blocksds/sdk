#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>

#include <fat.h>
#include <nds.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Configuration

#define CAMERA_NDMA_CHANNEL 1

// YUV<->RGB conversion

static inline int clamp(int val, int min, int max) {
	return val < min ? min : (val > max) ? max : val;
}

#define YUV_TO_R(Y, Cr) clamp(Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5), 0, 0xFF)
#define YUV_TO_G(Y, Cb, Cr) \
	clamp(Y - ((Cb >> 2) + (Cb >> 4) + (Cb >> 5)) - ((Cr >> 1) + (Cr >> 3) + (Cr >> 4) + (Cr >> 5)), 0, 0xFF)
#define YUV_TO_B(Y, Cb) clamp(Y + Cb + (Cb >> 1) + (Cb >> 2) + (Cb >> 6), 0, 0xFF)

ITCM_CODE
static void convertYuv422ToRgb(u16 *yuv, u8 *rgb, int width, int height) {
	for(int py = 0; py < height; py++) {
		for(int px = 0; px < width; px += 2) {
			u8 *val = (u8 *)(yuv + py * width + px);

			// Get YUV values
			int Y1 = val[0];
			int Cb = val[1] - 0x80;
			int Y2 = val[2];
			int Cr = val[3] - 0x80;

			u8 *dst = rgb + py * (width * 3) + px * 3;
			// First pixel R, G, B
			dst[0] = YUV_TO_R(Y1, Cr);
			dst[1] = YUV_TO_G(Y1, Cb, Cr);
			dst[2] = YUV_TO_B(Y1, Cb);
			// Second pixel R, G, B
			dst[3] = YUV_TO_R(Y2, Cr);
			dst[4] = YUV_TO_G(Y2, Cb, Cr);
			dst[5] = YUV_TO_B(Y2, Cb);
		}
	}
}

// Main program logic

int getImageNumber() {
	int highest = -1;

	DIR *pdir = opendir("/DCIM/100DSI00");
	if(pdir == NULL) {
		printf("Unable to open directory");
		return -1;
	} else {
		while(true) {
			struct dirent *pent = readdir(pdir);
			if(pent == NULL)
				break;

			if(strncmp(pent->d_name, "IMG_", 4) == 0) {
				int val = atoi(pent->d_name + 4);
				if(val > highest)
					highest = val;
			}
		}
		closedir(pdir);
	}

	return highest + 1;
}

static void exitOnError(void) {
	while (!(keysDown() & KEY_START)) {
		swiWaitForVBlank();
		scanKeys();
	}
	exit(1);
}

int main(int argc, char **argv) {
	// Set up console
	consoleDemoInit();
	vramSetBankA(VRAM_A_MAIN_BG);
	videoSetMode(MODE_5_2D);
	int bg3Main = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 1, 0);

	printf("Camera Test\n");

	if (!isDSiMode()) {
		printf("Must launch in DSi mode!\n");
		return exitOnError();
	}

	bool fatInited = fatInitDefault();
	if(fatInited) {
		mkdir("/DCIM", 0777);
		mkdir("/DCIM/100DSI00", 0777);
	} else {
		printf("FAT init failed, photos cannot\nbe saved.\n");
	}

	printf("Initializing camera...\n");
	if (!cameraInit()) {
		printf("Camera initialization failed!\n");
		return exitOnError();
	}

	printf("Selecting camera...\n");
	u8 camera = CAMERA_OUTER;
	if (!cameraSelect(camera)) {
		printf("Camera selection failed!\n");
		return exitOnError();
	}

	printf("\nA to swap");
	if(fatInited) printf(", L or R to take photo\n");
	printf("\n");

	while (true) {
		u16 pressed;
		do {
			swiWaitForVBlank();
			// If no preview transfer ongoing, start a new preview transfer.
			if(!ndmaBusy(CAMERA_NDMA_CHANNEL) || !cameraTransferActive()) {
				cameraStartTransfer(bgGetGfxPtr(bg3Main), MCUREG_APT_SEQ_CMD_PREVIEW, CAMERA_NDMA_CHANNEL);
			}
			scanKeys();
			pressed = keysDown();
		} while(!pressed);

		if(pressed & KEY_A) {
			// Wait for previous transfer to finish
			while(ndmaBusy(CAMERA_NDMA_CHANNEL))
				swiWaitForVBlank();
			cameraStopTransfer();

			// Switch camera
			camera = camera == CAMERA_INNER ? CAMERA_OUTER : CAMERA_INNER;
			if (!cameraSelect(camera)) {
				printf("Camera selection failed!\n");
				return exitOnError();
			}

			printf("Swapped to %s camera\n", camera == CAMERA_INNER ? "inner" : "outer");
		} else if(fatInited && (pressed & (KEY_L | KEY_R))) {
			printf("Capturing... ");

			// Wait for previous transfer to finish
			while(ndmaBusy(CAMERA_NDMA_CHANNEL))
				swiWaitForVBlank();

			u16 *yuv = (u16 *)malloc(640 * 480 * sizeof(u16));
			u8 *rgb = (u8 *)malloc(640 * 480 * 3);
			if (yuv == NULL || rgb == NULL) {
				printf("Could not allocate capture arrays!\n");
				if (yuv != NULL) free(yuv);
				if (rgb != NULL) free(rgb);
				continue;
			}

			// Run a capture transfer.
			cameraStartTransfer(yuv, MCUREG_APT_SEQ_CMD_CAPTURE, CAMERA_NDMA_CHANNEL);
			while(ndmaBusy(CAMERA_NDMA_CHANNEL))
				swiWaitForVBlank();
			cameraStopTransfer();

			// Convert YUV data to RGB.
			printf("Done!\nSaving PNG..");
			convertYuv422ToRgb(yuv, rgb, 640, 480);
			free(yuv);
			printf(". ");

			// Save RGB data as PNG file.
			char imgName[32];
			sprintf(imgName, "/DCIM/100DSI00/IMG_%04d.PNG", getImageNumber());
			stbi_write_png(imgName, 640, 480, 3, rgb, 640 * 3);
			free(rgb);

			printf("Done!\nSaved to:\n%s\n\n", imgName);
		} else if(pressed & KEY_START) {
			while(ndmaBusy(CAMERA_NDMA_CHANNEL))
				swiWaitForVBlank();
			break;
		}
	}

	cameraDeinit();
	return 0;
}
