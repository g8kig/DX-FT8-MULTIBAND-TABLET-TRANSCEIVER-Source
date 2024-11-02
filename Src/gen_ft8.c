/*
 * gen_ft8.c
 *
 *  Created on: Oct 24, 2019
 *      Author: user
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "pack.h"
#include "encode.h"
#include "constants.h"

#include "gen_ft8.h"

#include <stdio.h>

#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of device I/O functions */
#include "stdio.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery.h"

#include "stm32746g_discovery_lcd.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "arm_math.h"
#include <string.h>
#include "decode_ft8.h"
#include "Display.h"
#include "log_file.h"
#include "traffic_manager.h"
#include "ADIF.h"

#include "button.h"

char Target_Frequency[FREQUENCY_SIZE]; // Seven character frequency  + /0
char Locator[LOCATOR_SIZE]; // four character locator  + /0
char Station_Call[CALL_SIZE]; //six character call sign + /0
char Target_Call[CALL_SIZE]; //six character call sign + /0
char Target_Locator[LOCATOR_SIZE]; // four character locator  + /0
int Target_RSL; // four character RSL  + /0
char CQ_Target_Call[CALL_SIZE];

static char reply_message[CALL_SIZE + CALL_SIZE + REPORT_SIZE];
int reply_message_count;

static uint8_t isInitialized = 0;

/* Fatfs structure */
static FATFS FS;
static FIL fil;

const char CQ[] = "CQ";
const char seventy_three[] = "RR73";
const uint8_t blank[] = "                      ";

void set_cq(void) {
	char message[sizeof(CQ) + CALL_SIZE + LOCATOR_SIZE];
	uint8_t packed[K_BYTES];

	sprintf(message, "%s %s %s", CQ, Station_Call, Locator);

	pack77(message, packed);
	genft8(packed, tones);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(240, 240, blank, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(240, 240, (const uint8_t*) message, LEFT_MODE);

}

static int in_range(int num, int min, int max) {
	if (num < min)
		return min;
	if (num > max)
		return max;
	return num;
}

void set_reply(uint16_t index) {

	uint8_t packed[K_BYTES];
	char RSL[REPORT_SIZE];

	itoa(in_range(Target_RSL, -999, 9999), RSL, 10);

	if (index == 0)
		sprintf(reply_message, "%s %s %s", Target_Call, Station_Call, RSL);
	else if (index == 1) {
		sprintf(reply_message, "%s %s %s", Target_Call, Station_Call, seventy_three);
		write_ADIF_Log();
	}

	strcpy(current_Beacon_xmit_message, reply_message);
	update_Beacon_log_display(1);

	pack77(reply_message, packed);
	genft8(packed, tones);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(240, 240, blank, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(240, 240, (const uint8_t*) reply_message,
			LEFT_MODE);
}

typedef struct {
	char m[MESSAGE_SIZE];
} Message_T;

static Message_T xmit_messages[XMIT_MESSAGE_COUNT];

void compose_messages(void) {
	char RSL[REPORT_SIZE];

	itoa(in_range(Target_RSL, -999, 9999), RSL, 10);

	sprintf(xmit_messages[0].m, "%s %s %s", Target_Call, Station_Call, Locator);
	sprintf(xmit_messages[1].m, "%s %s R%s", Target_Call, Station_Call, RSL);
	sprintf(xmit_messages[2].m, "%s %s %s", Target_Call, Station_Call, seventy_three);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(240, 240, (const uint8_t*) xmit_messages[0].m, LEFT_MODE);
}

void que_message(int index) {

	uint8_t packed[K_BYTES];

	pack77(xmit_messages[index].m, packed);
	genft8(packed, tones);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(240, 220, blank, LEFT_MODE);

	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(240, 220, (const uint8_t*) xmit_messages[index].m, LEFT_MODE);

	strcpy(current_QSO_xmit_message, xmit_messages[index].m);

	if (index == 2)
		write_ADIF_Log();
}

void clear_qued_message(void) {

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(240, 220, blank, LEFT_MODE);
}

void clear_xmit_messages(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(240, 240, blank, LEFT_MODE);
}

void Read_Station_File(void) {

	uint8_t i;
	char read_buffer[BUFFER_SIZE] = { 0 };

	f_mount(&FS, SDPath, 1);
	if (f_open(&fil, "StationData.txt", FA_OPEN_ALWAYS | FA_READ) == FR_OK) {
		char *Station_Data;

		f_lseek(&fil, 0);
		f_gets(read_buffer, BUFFER_SIZE - 1, &fil);
		i = strlen(read_buffer);
		read_buffer[i] = 0;

		Station_Data = strtok(read_buffer, ":");
		strcpy(Station_Call, Station_Data);
		Station_Data = strtok(NULL, ":");
		strcpy(Locator, Station_Data);

		f_close(&fil);
	}
}

void clear_reply_message_box(void) {

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(240, 40, 240, 215);
}

void SD_Initialize(void) {

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);

	if (isInitialized == 0) {
		if (BSP_SD_Init() == MSD_OK) {
			BSP_SD_ITConfig();
			isInitialized = 1;
			FATFS_LinkDriver(&SD_Driver, SDPath);
		} else {
			BSP_LCD_DisplayStringAt(0, 100, (const uint8_t*) "Insert SD.", LEFT_MODE);
			while (BSP_SD_IsDetected() != SD_PRESENT) ;
			BSP_LCD_DisplayStringAt(0, 100, (const uint8_t*) "Reboot Now.", LEFT_MODE);
		}
	}
}

