/*
 * adif_export_file.c
 *
 *  Created on: Oct 29, 2019
 *      Author: user
 */

/*
 * F7_SD_Support.c
 *
 *  Created on: Apr 10, 2017
 *      Author: w2ctx
 */

#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "button.h"
/* Include core modules */
#include "stm32fxxx_hal.h"

#include <stdio.h>

#include "ff.h"		/* Declarations of FatFs API */
#include "diskio.h" /* Declarations of device I/O functions */
#include "stdio.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery.h"

#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "DS3231.h"
#include "ADIF_Export_File.h"

/* Fatfs structure */
static FATFS FS;
static FIL LogFile;

void Open_ADIF_Export_File(void)
{
	f_mount(&FS, "SD:", 1);
	if (f_open(&LogFile, file_name_string,
			   FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND) == FR_OK)
	{

		if (f_size(&LogFile) == 0)
		{
			f_lseek(&LogFile, f_size(&LogFile));
			f_puts("ADIF EXPORT", &LogFile);
			f_puts("\n", &LogFile);
			f_puts("<eoh>", &LogFile);
			f_puts("\n", &LogFile);
		}
	}
	Close_ADIF_Export_File();
}

void Write_ADIF_Export_Data(const char *line)
{
	f_mount(&FS, "SD:", 1);
	if (f_open(&LogFile, file_name_string,
				FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND) == FR_OK)
	{
		f_lseek(&LogFile, f_size(&LogFile));
		f_puts(line, &LogFile);
		f_puts("\n", &LogFile);
	}
	Close_ADIF_Export_File();
}

void Close_ADIF_Export_File(void)
{
	f_close(&LogFile);
}
