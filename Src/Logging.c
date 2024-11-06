/*
 * logging.c
 *
 *  Created on: Nov, 6 2024
 *      Author: Paul
 */

#include <stdio.h>

#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery.h"
#include "stm32fxxx_hal.h"
#include "ff.h"		/* Declarations of FatFs API */
#include "diskio.h" /* Declarations of device I/O functions */

/* Fatfs structure */
static FATFS FS;
static FIL LogFile;

int Open_Logging_File(void)
{
	f_mount(&FS, "SD:", 1);
	return f_open(&LogFile,
			"Logfile.txt",
			FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);
}

void Write_Logging_File(const char *line)
{
	f_lseek(&LogFile, f_size(&LogFile));
	f_puts(line, &LogFile);
}

void Close_Logging_File(void)
{
	f_close(&LogFile);
}
