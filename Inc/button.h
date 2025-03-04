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
#define button_bar_width 55
#define button_mode_width 45

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

extern int Band_Minimum;
extern int BTS_Status;

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

enum BandIndex
{
	_40M = 0,
	_30M = 1,
	_20M = 2,
	_17M = 3,
	_15M = 4,
	_12M = 5,
	_10M = 6,
	NumBands = 7
};

enum ButtonIds
{
	Clear = 0,
	QSOBeacon,
	Tune,
	RxTx,
	CQFree,
	FixedReceived,
	Sync,
	GainDown,
	GainUp,
	FreqDown,
	FreqUp,
	BandDown,
	BandUp,
	TxCalibrate,
	SaveBand,
	SaveRTCTime,
	HourDown,
	HourUp,
	MinuteDown,
	MinuteUp,
	SecondDown,
	SecondUp,
	DayDown,
	DayUp,
	MonthDown,
	MonthUp,
	YearDown,
	YearUp,
	SaveRTCDate,
	CQ,
	CQSOTA,
	CQPOTA,
	CQQRP,
	CQDX,
	FreeText1,
	FreeText2,
	NumButtons = 36
};

extern int Tune_On; // 0 = Receive, 1 = Xmit Tune Signal
extern int Beacon_On;
extern int Arm_Tune;
extern int Auto_Sync;
extern int QSO_Fix;
extern uint16_t start_freq;
extern int BandIndex;
extern int Band_Minimum;
extern const FreqStruct sBand_Data[NumBands];
extern int AGC_Gain;

void drawButton(uint16_t button);
void checkButton(void);

void executeButton(uint16_t index);
void executeCalibrationButton(uint16_t index);
void xmit_sequence(void);
void receive_sequence(void);
void start_Si5351(void);

void PTT_Out_Init(void);
void PTT_Out_Set(void);
void PTT_Out_RST_Clr(void);
void RLY_Select_20to40(void);
void RLY_Select_10to17(void);
void Check_Board_Version(void);
void Init_BoardVersionInput(void);
void DeInit_BoardVersionInput(void);
void set_codec_input_gain(void);

extern ButtonStruct sButtonData[NumButtons];

void setup_Cal_Display(void);
void erase_Cal_Display(void);

void FT8_Sync(void);

void SelectFilterBlock();

#endif /* BUTTON_H_ */
