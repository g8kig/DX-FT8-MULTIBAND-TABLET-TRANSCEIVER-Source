/*
 * DS3231.h
 *
 *  Created on: Dec 25, 2019
 *      Author: user
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "arm_math.h"

typedef struct
{
	const char *Name;
	const unsigned char Minimum;
	const unsigned char Maximum;
	int8_t data;
} RTCStruct;

extern RTCStruct s_RTC_Data[];

#define DS3231_Address              0x68

#define DS3231_Read_addr            ((DS3231_Address << 1) | 0x01)
#define DS3231_Write_addr           ((DS3231_Address << 1) & 0xFE)

#define secondREG                   0x00
#define minuteREG                   0x01
#define hourREG                     0x02
#define dayREG                      0x03
#define dateREG                     0x04
#define monthREG                    0x05
#define yearREG                     0x06
#define alarm1secREG                0x07
#define alarm1minREG                0x08
#define alarm1hrREG                 0x09
#define alarm1dateREG               0x0A
#define alarm2minREG                0x0B
#define alarm2hrREG                 0x0C
#define alarm2dateREG               0x0D
#define controlREG                  0x0E
#define statusREG                   0x0F
#define ageoffsetREG                0x10
#define tempMSBREG                  0x11
#define tempLSBREG                  0x12

#define _24_hour_format             0
#define _12_hour_format             1
#define AM                          0
#define PM                          1

extern int RTC_Set_Flag;

void DS3231_Write(unsigned char address, unsigned char value);
void DS3231_init(void);
void getTime(unsigned char *hour, unsigned char *minute, unsigned char *second,
		short *am_pm, short hour_format);
void getDate(unsigned char *day_of_week, unsigned char *date,
		unsigned char *month, unsigned char *year);

void RTC_setTime(unsigned char hSet, unsigned char mSet, unsigned char sSet,
		short am_pm_state, short hour_format);
void RTC_setDate(unsigned char daySet, unsigned char dateSet,
		unsigned char monthSet, unsigned char yearSet);

void display_RealTime(int x, int y);
void display_Real_Date(int x, int y);

void make_Real_Time(char *time_string);
void make_Real_Date(char *date_string);

void load_RealTime(void);
void load_RealDate(void);

void display_RTC_TimeEdit(int x, int y);
void display_RTC_DateEdit(int x, int y);
void set_RTC_to_TimeEdit(void);
void set_RTC_to_DateEdit(void);

#endif /* DS3231_H_ */
