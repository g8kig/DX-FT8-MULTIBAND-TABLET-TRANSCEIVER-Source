/*
 * ADIF.c
 *
 *  Created on: Jun 18, 2023
 *      Author: Charley
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "defines.h"
#include "ADIF.h"
#include "ADIF_Export_File.h"
#include "gen_ft8.h"
#include "DS3231.h"
#include "button.h"

void write_ADIF_Log(void)
{
	if (sButtonData[4].state == 1)
	{
		static char log_line[LOG_LINE_SIZE];

		make_Real_Time();
		make_Real_Date();

		sprintf(display_frequency, "%s", sBand_Data[BandIndex].display);
		sprintf(log_line,
				"<call:7>%7s<gridsquare:4>%4s<mode:3>FT8<qso_date:8>%8s <time_on:6>%6s<freq:9>%9s<station_callsign:7>%7s<my_gridsquare:4>%4s <eor>",
				Target_Call, Target_Locator, log_rtc_date_string,
				log_rtc_time_string, display_frequency, Station_Call, Locator);
		Write_ADIF_Export_Data(log_line);
	}
}
