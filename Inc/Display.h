/*
 * Display.h
 *
 *  Created on: Feb 8, 2016
 *      Author: user
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "arm_math.h"

#define FFT_H 40

#define FFT_Resolution 6.25 // 8000/2/1280

extern int FT_8_TouchIndex;

extern uint16_t cursor;
extern char rtc_date_string[9];
extern char rtc_time_string[9];
extern int decode_flag;

extern int QSO_Xmit_Touch;
extern int FT8_Touch_Flag;

extern int LoadQSOCall_Flag;
extern int Num_QSOCalls;
extern char QSOCalls[21][10];
extern char current_QSO_receive_message[40];
extern char current_Beacon_receive_message[40];
extern char current_Beacon_xmit_message[40];
extern char current_QSO_xmit_message[40];

void show_variable(uint16_t x, uint16_t y, int variable);

void show_short(uint16_t x, uint16_t y, uint8_t variable);

void show_wide(uint16_t x, uint16_t y, int variable);

void show_UTC_time(uint16_t x, uint16_t y, int utc_hours, int utc_minutes,
				   int utc_seconds, int color);

void show_Real_Date(uint16_t x, uint16_t y, int date, int month, int year);

void setup_display(void);

void Process_Touch(void);

void Display_WF(void);

void Set_Cursor_Frequency(void);

void update_log_display(int mode);

void clear_log_messages(void);

void clear_Beacon_log_messages(void);

void update_Beacon_log_display(int mode);

uint16_t testButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

#endif /* DISPLAY_H_ */
