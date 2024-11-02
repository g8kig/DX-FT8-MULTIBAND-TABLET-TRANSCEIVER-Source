/*
 * decode_ft8.h
 *
 *  Created on: Nov 2, 2019
 *      Author: user
 */

#ifndef DECODE_FT8_H_
#define DECODE_FT8_H_

extern int num_CQ_calls;
extern int num_calls_to_CQ_station;

typedef struct {
    char field1[FIELD1_SIZE];
    char field2[FIELD2_SIZE];
    char field3[FIELD3_SIZE];
    int  freq_hz;
    int  sync_score;
    int  snr;
    char target[LOCATOR_SIZE];
    int  slot;
} Decode;

typedef struct {
	char message[MESSAGE_SIZE];
	int text_color;
} display_message;

typedef struct {

	int number_times_called;
	char call[FIELD2_SIZE];
	char locator[LOCATOR_SIZE];
	int RSL;
	int RR73;

} Calling_Station;

typedef struct {
	char decode_time[DECODE_TIME_SIZE];
	char call[CALL_SIZE];
	int distance;
	int snr;
	int freq_hz;
} CQ_Station;

extern int Auto_QSO_State;

int Check_Calling_Stations(int num_decoded, int reply_state);
void Check_CQ_Stations(int num_decoded);
void clear_CQ_List_box(void);
void display_messages(int decoded_messages);
int Check_CQ_Calling_Stations(int num_decoded, int reply_state);
void clear_CQ_List_box(void);
void clear_messages(void);
void process_selected_Station(int stations_decoded, int TouchIndex);
int Check_QSO_Calling_Stations(int num_decoded, int reply_state);
void clear_log_stored_data(void);
void set_QSO_Xmit_Freq(int freq);

#endif /* DECODE_FT8_H_ */
