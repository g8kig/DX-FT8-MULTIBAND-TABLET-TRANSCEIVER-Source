/*
 * button.h
 *
 *  Created on: Feb 18, 2016
 *      Author: user
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "arm_math.h"
#include "fonts.h"

#define line0 10
#define line1 210
#define line2 240
#define line3 180

#define text_1 226
#define text_2 260

#define button_width 60

#define line0 10
#define line1 210
#define line2 240
#define line3 180

#define RTC_line0 40
#define RTC_line1 70
#define RTC_line2 100
#define RTC_line3 130
#define RTC_line4 160
#define RTC_line5 190

#define RTC_Sub 10
#define RTC_Button 100
#define RTC_Add 180

typedef struct
{
	char *text0;
	char *text1;
	char *blank;
	int Active;
	int Displayed;
	int state;
	const uint16_t x;
	const uint16_t y;
	const uint16_t w;
	const uint16_t h;

} ButtonStruct;

typedef struct
{
	uint16_t Frequency;
	char *display;
} FreqStruct;

extern int Tune_On; // 0 = Receive, 1 = Xmit Tune Signal
extern int Beacon_On;
extern int Arm_Tune;
extern int Auto_Sync;
extern uint16_t start_freq;
extern int BandIndex;
extern FreqStruct sBand_Data[];
extern int AGC_Gain;
extern int ADC_DVC_Gain;
extern int ADC_DVC_Off;
extern char display_frequency[];

uint16_t testButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

void drawButton(uint16_t i);
void drawMenu(uint16_t i);
void checkButton(void);

void executeButton(uint16_t index);
void executeCalibrationButton(uint16_t index);
void PTT_Out_Init(void);
void Execute_PTT(void);
void xmit_sequence(void);
void receive_sequence(void);
void start_Si5351(void);

void PTT_Out_Init(void);
void PTT_Out_Set(void);
void PTT_Out_RST_Clr(void);
void set_codec_input_gain(void);

extern ButtonStruct sButtonData[];

void setup_Cal_Display(void);
void erase_Cal_Display(void);

void FT8_Sync(void);

#endif /* BUTTON_H_ */
