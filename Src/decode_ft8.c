/*
 * decode_ft8.c
 *
 *  Created on: Sep 16, 2019
 *      Author: user
 */

#include <ADIF_Export_File.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <Display.h>

#include "defines.h"
#include "gen_ft8.h"
#include "unpack.h"
#include "ldpc.h"
#include "decode.h"
#include "constants.h"
#include "encode.h"
#include "button.h"
#include "main.h"

#include "Process_DSP.h"
#include "stm32746g_discovery_lcd.h"
#include "decode_ft8.h"
#include "traffic_manager.h"
#include "ADIF.h"
#include "DS3231.h"
#include "Logging.h"

#define LDPC_ITERATIONS 20
#define MINIMUM_SCORE 40 // Minimum sync score threshold for candidates

static int validate_locator(const char locator[]);
static int strindex(const char s[], const char t[]);

static Decode new_decoded[DECODED_MESSAGE_COUNT]; // chh 27 Feb
extern char current_QSO_receive_message[MESSAGE_SIZE];
extern char current_Beacon_receive_message[MESSAGE_SIZE];

static display_message display[DISPLAY_MESSAGE_COUNT];

static Calling_Station Answer_CQ[ANSWER_CQ_COUNT]; //

static int num_calls; // number of unique calling stations

int ft8_decode(void)
{
	// Find top candidates by Costas sync score and localize them in time and frequency
	Candidate candidate_list[CANDIDATE_COUNT];

	int num_candidates = find_sync(export_fft_power, ft8_msg_samples,
								   ft8_buffer, kCostas_map, CANDIDATE_COUNT, candidate_list,
								   MINIMUM_SCORE);
	char decoded[DECODED_MESSAGE_COUNT][MESSAGE_SIZE];

	const float fsk_dev = 6.25f; // tone deviation in Hz and symbol rate

	// Go over candidates and attempt to decode messages
	int num_decoded = 0;

	for (int idx = 0; idx < num_candidates; ++idx)
	{
		Candidate cand = candidate_list[idx];
		float freq_hz =
			((float)cand.freq_offset + (float)cand.freq_sub / 2.0f) * fsk_dev;

		float log174[N];
		extract_likelihood(export_fft_power, ft8_buffer, cand, kGray_map,
						   log174);

		// bp_decode() produces better decodes, uses way less memory
		uint8_t plain[N];
		int n_errors = 0;
		bp_decode(log174, LDPC_ITERATIONS, plain, &n_errors);

		if (n_errors > 0)
			continue;

		// Extract payload + CRC (first K bits)
		uint8_t a91[K_BYTES];
		pack_bits(plain, K, a91);

		// Extract CRC and check it
		uint16_t chksum = ((a91[9] & 0x07) << 11) | (a91[10] << 3) | (a91[11] >> 5);
		a91[9] &= 0xF8;
		a91[10] = 0;
		a91[11] = 0;
		uint16_t chksum2 = crc(a91, 96 - 14);
		if (chksum != chksum2)
			continue;

		char message[MESSAGE_SIZE];

		char field1[FIELD1_SIZE];
		char field2[FIELD2_SIZE];
		char field3[FIELD3_SIZE];
		int rc = unpack77_fields(a91, field1, field2, field3);
		if (rc < 0)
			continue;

		sprintf(message, "%s %s %s ", field1, field2, field3);

		_Bool found = false;
		for (int i = 0; i < num_decoded; ++i)
		{
			if (0 == strcmp(decoded[i], message))
			{
				found = true;
				break;
			}
		}

		if (!found && num_decoded < DECODED_MESSAGE_COUNT)
		{
			// Ignore 'spaceship' call signs
			if (field1[0] == '<')
				continue;
			// Also ignore call signs that are too large
			if (field2[0] == '<' || strlen(field2) >= CALL_SIZE)
				continue;
			// Also ignore locators that are too large
			if (strlen(field3) >= LOCATOR_SIZE)
				continue;

			strcpy(decoded[num_decoded], message);

			new_decoded[num_decoded].sync_score = cand.score;
			new_decoded[num_decoded].freq_hz = (int)freq_hz;

			strcpy(new_decoded[num_decoded].field1, field1);
			strcpy(new_decoded[num_decoded].field2, field2);
			strcpy(new_decoded[num_decoded].field3, field3);

			new_decoded[num_decoded].slot = slot_state;
			new_decoded[num_decoded].snr = (int)((new_decoded[num_decoded].sync_score - 160.0f)) / 6.0f;

			if (validate_locator(field3) == 1)
			{
				strcpy(new_decoded[num_decoded].target, field3);
			}
			else
			{
				// Invalid locator
				new_decoded[num_decoded].target[0] = 0;
			}

			++num_decoded;
		}
	} // End of big decode loop
	return num_decoded;
}

void display_messages(int num_decoded)
{
	const char CQ[] = "CQ";

	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, FFT_H, 240, 200);
	BSP_LCD_SetFont(&Font16);

	for (int i = 0; i < num_decoded && i < DISPLAY_MESSAGE_COUNT; i++)
	{
		const char *field1 = new_decoded[i].field1;
		const char *field2 = new_decoded[i].field2;
		const char *field3 = new_decoded[i].field3;

		sprintf(display[i].message, "%s %s %s", field1, field2, field3);

		if (strcmp(CQ, field1) == 0)
			display[i].text_color = 1;
		else
			display[i].text_color = 0;
	}

	for (int j = 0; j < num_decoded && j < DISPLAY_MESSAGE_COUNT; j++)
	{
		if (display[j].text_color == 0)
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		else
			BSP_LCD_SetTextColor(LCD_COLOR_GREEN);

		BSP_LCD_DisplayStringAt(0, 40 + j * 20, (const uint8_t *)display[j].message, LEFT_MODE);
	}
}

void clear_messages(void)
{
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(0, FFT_H, 240, 201);
}

static int validate_locator(const char locator[])
{
	uint8_t test = 0;

	uint8_t A1 = locator[0] - 65;
	uint8_t A2 = locator[1] - 65;
	uint8_t N1 = locator[2] - 48;
	uint8_t N2 = locator[3] - 48;

	if (A1 >= 0 && A1 <= 17)
		test++;
	if (A2 > 0 && A2 < 17)
		test++; // block RR73 Arctic and Antarctica
	if (N1 >= 0 && N1 <= 9)
		test++;
	if (N2 >= 0 && N2 <= 9)
		test++;

	if (test == 4)
		return 1;
	return 0;
}

void clear_log_stored_data(void)
{
	memset(Answer_CQ, 0, sizeof(Answer_CQ));
}

int Check_Calling_Stations(int num_decoded, int reply_state)
{
	int Beacon_Reply_Status = 0;

	for (int i = 0; i < num_decoded; i++)
	{ // check to see if being called
		int old_call;
		int old_call_address;

		if (strindex(new_decoded[i].field1, Station_Call) >= 0)
		{
			old_call = 0;

			for (int j = 0; j < num_calls; j++)
			{
				Calling_Station *stn = &Answer_CQ[j];
				if (strcmp(stn->call, new_decoded[i].field2) == 0)
				{
					old_call = ++stn->number_times_called;
					old_call_address = j;
				}
			}

			const char *field1 = new_decoded[i].field1;
			const char *field2 = new_decoded[i].field2;
			const char *field3 = new_decoded[i].field3;

			if (old_call == 0)
			{
				sprintf(current_Beacon_receive_message, " %s %s %s", field1, field2, field3);
				sprintf(current_QSO_receive_message, " %s %s %s", field1, field2, field3);

				if (Beacon_On == 1)
					update_Beacon_log_display(0);
				else if (Beacon_On == 0)
					update_log_display(0);

				memcpy(Target_Call, field2, CALL_SIZE);
				Target_Call[CALL_SIZE - 1] = 0;
				Target_RSL = new_decoded[i].snr;

				if (Beacon_On == 1)
					set_reply(0);

				Beacon_Reply_Status = 1;

				Calling_Station *stn = &Answer_CQ[num_calls];
				strcpy(stn->call, new_decoded[i].field2);
				strcpy(stn->locator, new_decoded[i].target);
				stn->RSL = Target_RSL;

				num_calls++;
				break;
			}

			if (old_call >= 1)
			{

				sprintf(current_Beacon_receive_message, " %s %s %s", field1, field2, field3);
				sprintf(current_QSO_receive_message, " %s %s %s", field1, field2, field3);

				if (Beacon_On == 1)
				{
					update_Beacon_log_display(0);
				}
				else if (Beacon_On == 0)
				{
					update_log_display(0);
				}

				if (Answer_CQ[old_call_address].RR73 == 0)
				{
					memcpy(Target_Call, Answer_CQ[old_call_address].call, CALL_SIZE);
					Target_Call[CALL_SIZE - 1] = 0;
					strcpy(Target_Locator, Answer_CQ[old_call_address].locator);
					Target_RSL = Answer_CQ[old_call_address].RSL;

					if (Beacon_On == 1)
						set_reply(1);

					Answer_CQ[old_call_address].RR73 = 1;
					Beacon_Reply_Status = 1;
				}
				else
					Beacon_Reply_Status = 0;
			}
		} // check for station call
	} // check to see if being called

	return Beacon_Reply_Status;
}

void process_selected_Station(int stations_decoded, int TouchIndex)
{
	if (stations_decoded > 0 && TouchIndex <= stations_decoded)
	{
		const Decode *decode = &new_decoded[TouchIndex];

		memcpy(Target_Call, decode->field2, CALL_SIZE);
		Target_Call[CALL_SIZE - 1] = 0;

		strcpy(Target_Locator, decode->target);
		Target_RSL = decode->snr;
		target_slot = decode->slot;
		target_freq = decode->freq_hz;

		set_QSO_Xmit_Freq(target_freq);

		compose_messages();
		Auto_QSO_State = 1;
		stop_QSO_reply = 0;
	}

	FT8_Touch_Flag = 0;
}

void set_QSO_Xmit_Freq(int freq)
{
	freq = freq - ft8_min_freq;
	cursor = (uint16_t)((float)freq / FFT_Resolution);

	Set_Cursor_Frequency();
	show_variable(400, 25, (int)NCO_Frequency);
}

static int strindex(const char s[], const char t[])
{
	int result = -1;

	for (int i = 0; s[i] != 0; i++)
	{
		int k = 0;
		for (int j = i, k = 0; t[k] != 0 && s[j] == t[k]; j++, k++)
			;
		if (k > 0 && t[k] == 0)
			result = i;
	}
	return result;
}
