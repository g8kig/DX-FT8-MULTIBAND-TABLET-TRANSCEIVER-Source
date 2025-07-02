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
#include <ctype.h>
#include <stdbool.h>

extern "C"
{
#include "pack.h"
#include "encode.h"
#include "constants.h"
}

#include "gen_ft8.h"

#include "ff.h"		/* Declarations of FatFs API */
#include "diskio.h" /* Declarations of device I/O functions */
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "arm_math.h"
#include "decode_ft8.h"
#include "Display.h"
#include "log_file.h"
#include "traffic_manager.h"
#include "ADIF.h"

#include "button.h"
#include "ini.h"

char Station_Call[CALLSIGN_SIZE];	// seven character call sign (e.g. 3DA0XYZ) + optional /P + null terminator
char Locator[LOCATOR_SIZE];		// four character locator + null terminator
char Target_Call[CALLSIGN_SIZE];	// seven character call sign (e.g. 3DA0XYZ) + optional /P + null terminator
char Target_Locator[LOCATOR_SIZE]; // four character locator  + null terminator
int Station_RSL;

char reply_message[REPLY_MESSAGE_SIZE];

const int display_start_x = 240;
const int display_start_y = 240;
const int display_width = 230;

static uint8_t isInitialized = 0;

/* Fatfs structure */
static FATFS FS;
static FIL fil;
static FIL fil2;

static char blank[19];
static uint8_t blank_initialised = 0;

const char CQ[] = "CQ";
const char SOTA[] = "SOTA";
const char POTA[] = "POTA";
const char QRP[] = "QRP";
const char Beacon_73[] = "RR73";
const char QSO_73[] = "73";

static char Free_Text1[MESSAGE_SIZE];
static char Free_Text2[MESSAGE_SIZE];

void set_cq(void)
{
	char message[MESSAGE_SIZE];
	uint8_t packed[K_BYTES];
	if (Free_Index == 0)
	{
		const char *mode = NULL;
		switch (CQ_Mode_Index)
		{
		default:
		case 0:
			break;
		case 1:
			mode = SOTA;
			break;
		case 2:
			mode = POTA;
			break;
		case 3:
			mode = QRP;
			break;
		}

		if (mode == NULL)
		{
			sprintf(message, "%s %s %s", CQ, Station_Call, Locator);
		}
		else
		{
			sprintf(message, "%s %s %s %s", CQ, mode, Station_Call, Locator);
		}
	}
	else
	{
		switch (Free_Index)
		{
		default:
		case 0:
			break;
		case 1:
			strcpy(message, Free_Text1);
			break;
		case 2:
			strcpy(message, Free_Text2);
			break;
		}
	}

	pack77(message, packed);
	genft8(packed, tones);

	string_init(blank, sizeof(blank), &blank_initialised, ' ');
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)blank, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)message, LEFT_MODE);
}

static int in_range(int num, int min, int max)
{
	if (num < min)
		return min;
	if (num > max)
		return max;
	return num;
}

void set_reply(ReplyID replyId)
{
	uint8_t packed[K_BYTES];
	char RSL[5];

	if (replyId == Reply_RSL || replyId == Reply_R_RSL)
	{
		// compute the RSL for use by the next 'switch'
		itoa(in_range(Target_RSL, -999, 9999), RSL, 10);
	}

	switch (replyId)
	{
	case Reply_RSL:
		sprintf(reply_message, "%s %s %s", Target_Call, Station_Call, RSL);
		break;
	case Reply_R_RSL:
		sprintf(reply_message, "%s %s R%s", Target_Call, Station_Call, RSL);
		break;
	case Reply_Beacon_73:
		sprintf(reply_message, "%s %s %s", Target_Call, Station_Call, Beacon_73);
		break;
	case Reply_QSO_73:
		sprintf(reply_message, "%s %s %s", Target_Call, Station_Call, QSO_73);
		break;
	}

	strcpy(current_Beacon_xmit_message, reply_message);
	update_Beacon_log_display(1);

	pack77(reply_message, packed);
	genft8(packed, tones);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)blank, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)reply_message, LEFT_MODE);
}

static char xmit_messages[Que_Size][MESSAGE_SIZE];

void compose_messages(void)
{
	char RSL[5];
	itoa(in_range(Target_RSL, -999, 9999), RSL, 10);

	sprintf(xmit_messages[Que_Locator], "%s %s %s", Target_Call, Station_Call, Locator);
	sprintf(xmit_messages[Que_RSL], "%s %s R%s", Target_Call, Station_Call, RSL);
	sprintf(xmit_messages[Que_73], "%s %s %s", Target_Call, Station_Call, QSO_73);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)xmit_messages[Que_Locator], LEFT_MODE);
}

void queue_message(QueID queId)
{
	uint8_t packed[K_BYTES];

	const char *tx_msg = xmit_messages[queId];
	pack77(tx_msg, packed);
	genft8(packed, tones);

	string_init(blank, sizeof(blank), &blank_initialised, ' ');
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y - 20, (const uint8_t *)blank, LEFT_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y - 20, (const uint8_t *)tx_msg, LEFT_MODE);

	strcpy(current_QSO_xmit_message, tx_msg);

	if (queId == Que_73 && Station_RSL != 99)
		write_ADIF_Log();
}

void clear_queued_message(void)
{
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y - 20, (const uint8_t *)blank, LEFT_MODE);
}

void clear_xmit_messages(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(display_start_x, display_start_y, (const uint8_t *)blank, LEFT_MODE);
}

static void set_text(char *text, const char *source, int field_id)
{
	strcpy(text, source);
	for (size_t i = 0; i < strlen(text); ++i)
	{
		if (!isprint((int)text[i]))
		{
			text[0] = 0;
			break;
		}
	}

	if (field_id >= 0)
	{
		sButtonData[field_id].text0 = text;
		sButtonData[field_id].text1 = text;
	}
}

static const char *delimiters = ":;,";

static void validate_call()
{
	for (size_t i = 0; i < strlen(Station_Call); ++i)
	{
		if (!isprint((int)Station_Call[i]))
		{
			Station_Call[0] = 0;
			break;
		}
	}
}

static int setup_station_call(const char *call_part)
{
	int result = 0;
	if (call_part != NULL)
	{
		size_t i = strlen(call_part);
		result = i > 0 && i < sizeof(Station_Call) ? 1 : 0;
		if (result != 0)
		{
			strcpy(Station_Call, call_part);
			validate_call();
		}
	}
	return result;
}

static int setup_locator(const char *locator_part)
{
	int result = 0;
	if (locator_part != NULL)
	{
		size_t i = strlen(locator_part);
		result = i > 0 && i < sizeof(Locator) ? 1 : 0;
		if (result != 0)
			set_text(Locator, locator_part, -1);
	}
	return result;
}

static int setup_free_text(const char *free_text, int field_id)
{
	int result = 0;
	if (free_text != NULL)
	{
		size_t i = strlen(free_text);
		switch (field_id)
		{
		case FreeText1:
			result = i < sizeof(Free_Text1) ? 1 : 0;
			if (i > 0 && result != 0)
				set_text(Free_Text1, free_text, field_id);
			break;
		case FreeText2:
			result = i < sizeof(Free_Text2) ? 1 : 0;
			if (i > 0 && result != 0)
				set_text(Free_Text2, free_text, field_id);
			break;
		default:
			result = 1;
		}
	}
	return result;
}

void Read_Station_File(void)
{
	static const char *data_file_name = "StationData.txt";
 	static const char *ini_file_name  = "StationData.ini";

	Station_Call[0] = 0;
	Locator[0] = 0;
	Free_Text1[0] = 0;
	Free_Text2[0] = 0;

	f_mount(&FS, SDPath, 1);

	FILINFO filInfo;
	memset(&filInfo, 0, sizeof(FILINFO));
	if (f_stat(data_file_name, &filInfo) == FR_OK &&
		f_open(&fil, data_file_name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		char read_buffer[filInfo.fsize];
		char *call_part, *locator_part = NULL, *free_text1_part = NULL, *free_text2_part = NULL;
		f_lseek(&fil, 0);
		f_gets(read_buffer, filInfo.fsize, &fil);

		call_part = strtok(read_buffer, delimiters);
		if (call_part != NULL)
			locator_part = strtok(NULL, delimiters);
		if (locator_part != NULL)
			free_text1_part = strtok(NULL, delimiters);
		if (free_text1_part != NULL)
			free_text2_part = strtok(NULL, delimiters);

		setup_station_call(call_part);
		setup_locator(locator_part);
		setup_free_text(free_text1_part, FreeText1);
		setup_free_text(free_text2_part, FreeText2);
		f_close(&fil);
	}
	else
	{
		if (f_stat(ini_file_name, &filInfo) == FR_OK &&
			f_open(&fil2, ini_file_name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
		{
			char read_buffer[filInfo.fsize];
			f_lseek(&fil2, 0);

			unsigned bytes_read;
			if (f_read(&fil2, read_buffer, filInfo.fsize, &bytes_read) == FR_OK)
			{
				ini_data_t ini_data;
				parse_ini(read_buffer, bytes_read, &ini_data);

				const ini_section_t *section = get_ini_section(&ini_data, "Station");
				if (section != NULL)
				{
					setup_station_call(get_ini_value_from_section(section, "Call"));
					setup_locator(get_ini_value_from_section(section, "Locator"));
				}

				section = get_ini_section(&ini_data, "FreeText");
				if (section != NULL)
				{
					setup_free_text(get_ini_value_from_section(section, "1"), FreeText1);
					setup_free_text(get_ini_value_from_section(section, "2"), FreeText2);
				}

				section = get_ini_section(&ini_data, "BandData");
				if (section != NULL)
				{
					// see BandIndex
					static const char *bands[NumBands] = {"40", "30", "20", "17", "15", "12", "10"};
					for (int idx = _40M; idx <= _10M; ++idx)
					{
						const char *band_data = get_ini_value_from_section(section, bands[idx]);
						if (band_data != NULL)
						{
							size_t band_data_size = strlen(band_data) + 1;
							if (band_data_size < BAND_DATA_SIZE)
							{
								sBand_Data[idx].Frequency = (uint16_t)(atof(band_data) * 1000);
								memcpy(sBand_Data[idx].display, band_data, band_data_size);
							}
						}
					}
				}
			}

			f_close(&fil2);
		}
	}
}

void clear_reply_message_box(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(display_start_x, 40, display_width, 215);
}

void SD_Initialize(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 0, 480, 272);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);

	if (isInitialized == 0)
	{
		if (BSP_SD_Init() == MSD_OK)
		{
			BSP_SD_ITConfig();
			isInitialized = 1;
			FATFS_LinkDriver(&SD_Driver, SDPath);
		}
		else
		{
			BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Insert SD.", LEFT_MODE);
			while (BSP_SD_IsDetected() != SD_PRESENT)
			{
				HAL_Delay(100);
			}
			BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Reboot Now.", LEFT_MODE);
		}
	}

	uint8_t result = BSP_SDRAM_Init();
	if (result == SDRAM_ERROR)
	{
		BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Failed to initialise SDRAM", LEFT_MODE);
		for (;;)
		{
			HAL_Delay(100);
		}
	}
}
