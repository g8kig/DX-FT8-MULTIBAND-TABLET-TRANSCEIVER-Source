#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "Process_DSP.h"
#include "button.h"
#include "Display.h"
#include "gen_ft8.h"
#include "main.h"
#include "stdio.h"
#include "decode_ft8.h"
#include "WF_Table.h"

#define FFT_X 0
#define FFT_Y 1
#define FFT_W (ft8_buffer_size - ft8_min_bin)

int FT_8_TouchIndex;
uint16_t cursor = 192;
int decode_flag;
int FT8_Touch_Flag;
char current_QSO_receive_message[MESSAGE_SIZE];
char current_QSO_xmit_message[MESSAGE_SIZE];

const int log_start = 240;
const int log_width = 230;

static uint8_t WF_Bfr[FFT_H * FFT_W];

#define RTC_STRING_SIZE 9
static char rtc_date_string[RTC_STRING_SIZE];
static char rtc_time_string[RTC_STRING_SIZE];

#define MAX_LOG_MESSAGES 8
static display_message log_messages[MAX_LOG_MESSAGES];

#define MAX_BLANK_SIZE 21
static char blank[MAX_BLANK_SIZE];
static uint8_t blank_initialised = 0;

void update_log_display(int mode)
{
	for (int i = 0; i < MAX_LOG_MESSAGES - 1; i++)
	{
		strcpy(log_messages[i].message, log_messages[i + 1].message);
		log_messages[i].text_color = log_messages[i + 1].text_color;
	}

	if (mode == 0)
	{
		strcpy(log_messages[MAX_LOG_MESSAGES - 1].message,
			   current_QSO_receive_message);
		log_messages[MAX_LOG_MESSAGES - 1].text_color = 0;
	}
	else
	{
		strcpy(log_messages[MAX_LOG_MESSAGES - 1].message,
			   current_QSO_xmit_message);
		log_messages[MAX_LOG_MESSAGES - 1].text_color = 1;
	}

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(log_start, 40, log_width, 160);

	BSP_LCD_SetFont(&Font16);

	for (int i = 0; i < MAX_LOG_MESSAGES; i++)
	{
		if (log_messages[i].text_color == 0)
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
		else if (log_messages[i].text_color == 1)
			BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

		BSP_LCD_DisplayStringAt(log_start, 40 + i * 20,
								(const uint8_t *)log_messages[i].message, LEFT_MODE);
	}
}

void clear_log_messages(void)
{
	string_init(blank, sizeof(blank), &blank_initialised, ' ');

	for (int i = 0; i < MAX_LOG_MESSAGES; i++)
		strcpy(log_messages[i].message, blank);
}

char current_Beacon_receive_message[MESSAGE_SIZE];
char current_Beacon_xmit_message[MESSAGE_SIZE];

#define MAX_BEACON_LOG_MESSAGES 8
static display_message Beacon_log_messages[MAX_BEACON_LOG_MESSAGES];

void update_Beacon_log_display(int mode)
{
	for (int i = 0; i < MAX_BEACON_LOG_MESSAGES - 1; i++)
	{
		strcpy(Beacon_log_messages[i].message, Beacon_log_messages[i + 1].message);
		Beacon_log_messages[i].text_color = Beacon_log_messages[i + 1].text_color;
	}

	if (mode)
	{
		strcpy(Beacon_log_messages[MAX_BEACON_LOG_MESSAGES - 1].message, current_Beacon_xmit_message);
		Beacon_log_messages[MAX_BEACON_LOG_MESSAGES - 1].text_color = 1;
	}
	else
	{
		strcpy(Beacon_log_messages[MAX_BEACON_LOG_MESSAGES - 1].message, current_Beacon_receive_message);
		Beacon_log_messages[MAX_BEACON_LOG_MESSAGES - 1].text_color = 0;
	}

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(log_start, 40, log_width, 200);
	BSP_LCD_SetFont(&Font16);

	for (int i = 0; i < MAX_BEACON_LOG_MESSAGES; i++)
	{
		if (Beacon_log_messages[i].text_color == 0)
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
		else
			BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

		BSP_LCD_DisplayStringAt(log_start, 40 + i * 20,
								(const uint8_t *)Beacon_log_messages[i].message, LEFT_MODE);
	}
}

void clear_Beacon_log_messages(void)
{
	string_init(blank, sizeof(blank), &blank_initialised, ' ');

	for (int i = 0; i < MAX_BEACON_LOG_MESSAGES; i++)
		strcpy(Beacon_log_messages[i].message, blank);
}

void show_wide(uint16_t x, uint16_t y, int variable)
{
	char string[7]; // print format stuff
	sprintf(string, "%6i", variable);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_DisplayStringAt(x, y, (const uint8_t *)string, LEFT_MODE);
}

void show_variable(uint16_t x, uint16_t y, int variable)
{
	char string[5]; // print format stuff
	sprintf(string, "%4i", variable);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_DisplayStringAt(x, y, (const uint8_t *)string, LEFT_MODE);
}

void show_short(uint16_t x, uint16_t y, uint8_t variable)
{
	char string[4]; // print format stuff
	sprintf(string, "%2i", variable);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_DisplayStringAt(x, y, (const uint8_t *)string, CENTER_MODE);
}

void show_Real_Time(uint16_t x, uint16_t y, int hours, int minutes, int seconds, int color)
{
	sprintf(rtc_time_string, "%02i:%02i:%02i", hours, minutes, seconds);
	BSP_LCD_SetFont(&Font16);

	if (color)
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	else
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);

	BSP_LCD_DisplayStringAt(x, y, (const uint8_t *)rtc_time_string, LEFT_MODE);
}

void show_Real_Date(uint16_t x, uint16_t y, int date, int month, int year)
{
	sprintf(rtc_date_string, "%02i/%02i/%02i", date, month, year);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_DisplayStringAt(x, y, (const uint8_t *)rtc_date_string, LEFT_MODE);
}

void setup_display(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, 0, 480, 272);

	BSP_LCD_SetFont(&Font16);

	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

	BSP_LCD_DisplayStringAt(0, 60, (const uint8_t *)"DX FT8: A FT8 Xceiver", LEFT_MODE);
	BSP_LCD_DisplayStringAt(50, 80, (const uint8_t *)"Hardware: V2.0", LEFT_MODE);
	BSP_LCD_DisplayStringAt(50, 100, (const uint8_t *)"Firmware: V1.9.7", LEFT_MODE);
	BSP_LCD_DisplayStringAt(50, 120, (const uint8_t *)"W5BAA - WB2CBA", LEFT_MODE);

	BSP_LCD_DisplayStringAt(50, 160,
							(Band_Minimum == _20M)
								? (const uint8_t *)"Five Band Board"
								: (const uint8_t *)"Seven Band Board",
							LEFT_MODE);

	if (strlen(Station_Call) == 0 || strlen(Locator) == 0)
	{
		char buffer[256];
		sprintf(buffer, "Invalid Call '%s' or Locator '%s'", Station_Call, Locator);
		BSP_LCD_DisplayStringAt(0, 180, (const uint8_t *)buffer, LEFT_MODE);
	}
	else
	{
		char callOrLocator[32];
		sprintf(callOrLocator, "Call    : %s", Station_Call);
		BSP_LCD_DisplayStringAt(50, 180, (const uint8_t *)callOrLocator, LEFT_MODE);
		sprintf(callOrLocator, "Locator : %s", Locator);
		BSP_LCD_DisplayStringAt(50, 200, (const uint8_t *)callOrLocator, LEFT_MODE);
	}

	for (int buttonId = Clear; buttonId <= FreqUp; ++buttonId)
		drawButton(buttonId);
}

void Set_Cursor_Frequency(void)
{
	NCO_Frequency = (double)((float)cursor * FFT_Resolution + ft8_min_freq);
}

static uint16_t valx, valy;

uint16_t testButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	y = y + 15; // compensate for draw offset

	if ((valx < x + w && valx > x) && (valy > y && valy < y + h))
	{
		return 1;
	}
	return 0;
}

static uint8_t FFT_Touch()
{
	if ((valx > FFT_X && valx < FFT_X + FFT_W) && (valy > FFT_Y && valy < 30))
		return 1;
	return 0;
}

static uint8_t LCD_Backlight_Touch()
{
	// Touch on the 'frequency' area top right of the display
	if ((valx > FFT_W) && (valy < 25))
		return 1;
	return 0;
}

static uint8_t FT8_Touch(void)
{
	if ((valx > 0 && valx < 240) && (valy > 40 && valy < 240))
	{
		int y_test = valy - 40;

		FT_8_TouchIndex = y_test / 20;

		return 1;
	}
	return 0;
}

void Process_Touch(void)
{
	static uint8_t touch_detected = 0;
	static uint8_t display_on = 1;

	TS_StateTypeDef TS_State;
	BSP_TS_GetState(&TS_State);

	if (!Tune_On && !xmit_flag && !Beacon_On)
		sButtonData[Sync].state = 0;
	else
		sButtonData[Sync].state = 1;

	if (!touch_detected)
	{
		// Display touched?
		if (TS_State.touchDetected)
		{
			valx = (uint16_t)TS_State.touchX[0];
			valy = (uint16_t)TS_State.touchY[0];

			FT8_Touch_Flag = FT8_Touch();

			touch_detected = 1;
		}
	}
	// Display touch lifted
	else if (!TS_State.touchDetected)
	{
		uint8_t turn_backlight_on = 1;

		touch_detected = 0;
		if (display_on)
		{
			// In the FFT area?
			if (FFT_Touch())
			{
				cursor = (valx - FFT_X);
				if (cursor > FFT_W - 8)
					cursor = FFT_W - 8;

				NCO_Frequency = (double)(cursor + ft8_min_bin) * FFT_Resolution;
				show_variable(400, 25, (int)NCO_Frequency);
			}
			// in the backlight area?
			else if (LCD_Backlight_Touch())
			{
				BSP_LCD_DisplayOff();
				turn_backlight_on = display_on = 0;
			}
			else
			{
				checkButton();
			}
		}

		if (turn_backlight_on && !display_on)
		{
			BSP_LCD_DisplayOn();
			display_on = 1;
		}
	}
}

const int max_noise_count = 3;
const int max_noise_free_sets_count = 3;

static int noise_free_sets_count = 0;

void Display_WF(void)
{
	const int byte_count_to_last_line = FFT_W * (FFT_H - 1);

	// shift data in memory by one time step (FFT_W)
	memmove(WF_Bfr, &WF_Bfr[FFT_W], byte_count_to_last_line);

	// set the new data in the last line unless a marker line is to be drawn
	for (int x = 0; x < FFT_W; x++)
	{
		WF_Bfr[byte_count_to_last_line + x] = (ft8_marker) ? marker_line_colour_index : FFT_Buffer[x + ft8_min_bin];
	}

	// Draw the waterfall from the bottom to the top
	uint8_t *ptr = &WF_Bfr[byte_count_to_last_line];
	for (int y = FFT_H - 1; y >= 0; y--)
	{
		for (int x = 0; x < FFT_W; x++)
		{
			// Each FFT datum is 6.25hz, the transmit bandwidth is 50Hz (= 8 pixels)
			if ((x == cursor) || (x == cursor + 8))
			{
				BSP_LCD_DrawPixel(x, y, xmit_flag ? LCD_COLOR_RED : LCD_COLOR_LIGHTGRAY);
			}
			else
			{
				BSP_LCD_DrawPixel(x, y, WFPalette[*ptr]);
			}
			ptr++;
		}

		ptr -= (FFT_W * 2);
	}

	if (!ft8_marker && Auto_Sync)
	{
		// count the amount of noise seen in the FFT
		int noise_count = 0;
		ptr = &WF_Bfr[byte_count_to_last_line];
		for (int x = 0; x < FFT_W; x++)
		{
			if ((*ptr++ > 0) && (++noise_count >= max_noise_count))
				break;
		}

		// if less than the maximum noise count in the FFT
		if (noise_count < max_noise_count)
		{
			// if sufficient noise free FFT data is seen it's time to synchronise
			if (++noise_free_sets_count >= max_noise_free_sets_count)
			{
				FT8_Sync();
				Auto_Sync = 0;
				noise_free_sets_count = 0;
				sButtonData[Sync].state = 0;
				drawButton(Sync);
			}
		}
		else
		{
			noise_free_sets_count = 0;
		}
	}

	ft8_marker = 0;
}
