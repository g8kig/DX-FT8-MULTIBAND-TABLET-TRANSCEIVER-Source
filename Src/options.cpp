#include <stdio.h>
#include <stdlib.h>

#include "Display.h"
#include "options.h"
#include "SDR_Audio.h"
#include "Codec_Gains.h"
#include "main.h"
#include "button.h"
#include "DS3231.h"

#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery.h"
#include "stm32fxxx_hal.h"

#include "ff.h"		/* Declarations of FatFs API */
#include "diskio.h" /* Declarations of device I/O functions */

#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "DS3231.h"

/* Fatfs structure */
static FATFS SDFatFs;	/* File system object for SD card logical drive */
static FIL MyFile;		/* File object */

char SDPath[4]; /* SD card logical drive path */

// Order must match OptionNumber in options.h
static OptionStruct s_optionsData[] = {
	{
		/*Name*/ "  Band_Index ", // opt0
		/*Init*/ _20M,			  // Set default band to 20 meters for 5 Band Board Protection
		/*Min */ 0,
		/*Max */ 4,
		/*Rate*/ 1,
		/*Data*/ 0,
	},
	{
		/*Name*/ "  Logging_State ", // opt1
		/*Init*/ 1,					 // Logging is by default, on
		/*Min */ 0,
		/*Max */ 1,
		/*Rate*/ 1,
		/*Data*/ 0,
	}};

int16_t Options_GetValue(int optionIdx)
{
	return s_optionsData[optionIdx].CurrentValue;
}

void Options_SetValue(int optionIdx, int16_t newValue)
{
	s_optionsData[optionIdx].CurrentValue = newValue;
}

// Initialization
int16_t Options_Initialize(void)
{
	int16_t result = 0;
	FRESULT fres = f_mount(&SDFatFs, SDPath, 1);
	if (fres == FR_OK)
		fres = f_open(&MyFile, "SaveParams.txt", FA_READ);
	if (fres == FR_OK)
	{
		f_close(&MyFile);
		result = Options_ReadFromMicroSD();
	}
	else
	{
		Options_ResetToDefaults();
		result = Options_WriteToMicroSD();
	}

	BandIndex = Options_GetValue(OPTION_Band_Index);
	Logging_State = Options_GetValue(OPTION_Logging_State);
	start_freq = sBand_Data[BandIndex].Frequency;
	show_wide(380, 0, (int)start_freq);

	SelectFilterBlock();
	return result;
}

void Options_ResetToDefaults(void)
{
	for (int i = 0; i < NUM_OPTIONS; i++)
	{
		Options_SetValue(i, s_optionsData[i].Initial);
	}
}

// MicroSD Access
int16_t Options_WriteToMicroSD(void)
{
	int16_t result = 1;
	for (int i = 0; i < NUM_OPTIONS; i++)
	{
		if (Write_Int_MicroSD(i, Options_GetValue(i)) == 0)
			result = 0;
	}
	return result;
}

const int16_t badValue = 0xbaad;

int16_t Options_ReadFromMicroSD(void)
{
	int16_t result = 1;
	for (int i = 0; i < NUM_OPTIONS; i++)
	{
		int16_t newValue = Read_Int_MicroSD(i);
		if (newValue != badValue)
			Options_SetValue(i, newValue);
		else
			result = 0;
	}
	return result;
}

int16_t Options_StoreValue(int optionIdx)
{
	int16_t option_value;
	option_value = Options_GetValue(optionIdx);
	return Write_Int_MicroSD((int16_t)optionIdx, option_value);
}

void Options_SetMinimum(int newMinimum)
{
	s_optionsData[0].Minimum = newMinimum;
}

// Routine to write a integer value to the MicroSD starting at MicroSD address MicroSD_Addr
int16_t Write_Int_MicroSD(uint16_t DiskBlock, int16_t value)
{
	int16_t result = 0;
	char read_buffer[32] = {0};
	FRESULT fres = f_mount(&SDFatFs, SDPath, 1);
	if (fres == FR_OK)
		fres = f_open(&MyFile, "SaveParams.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	if (fres == FR_OK)
	{
		fres = f_lseek(&MyFile, DiskBlock * 32);
		if (fres == FR_OK)
		{
			sprintf(read_buffer, "%2i", value);
			fres = (FRESULT)f_puts(read_buffer, &MyFile);
			if (fres > 0 && fres < sizeof(read_buffer))
				result = 1;
		}
		f_close(&MyFile);
	}
	return result;
}

int16_t Read_Int_MicroSD(uint16_t DiskBlock)
{
	int16_t result = badValue;
	char read_buffer[32] = {0};
	FRESULT fres = f_mount(&SDFatFs, SDPath, 1);
	if (fres == FR_OK)
		fres = f_open(&MyFile, "SaveParams.txt", FA_READ);
	if (fres == FR_OK)
		fres = f_lseek(&MyFile, DiskBlock * 32);
	if (fres == FR_OK)
		if (f_gets(read_buffer, 32, &MyFile) != NULL)
			result = atoi(read_buffer);
	f_close(&MyFile);
	return result;
}
