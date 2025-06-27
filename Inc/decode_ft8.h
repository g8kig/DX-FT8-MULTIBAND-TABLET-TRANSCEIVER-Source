/*
 * decode_ft8.h
 *
 *  Created on: Nov 2, 2019
 *      Author: user
 */

#ifndef DECODE_FT8_H_
#define DECODE_FT8_H_

extern int Auto_QSO_State;
extern int Station_RSL;
extern int Target_RSL;

#define DECODE_CALLSIGN_SIZE 14
#define DECODE_LOCATOR_SIZE 7
#define DISPLAY_MESSAGE_SIZE 22

typedef enum _Sequence
{
    Seq_RSL = 0,
    Seq_Locator
} Sequence;

typedef struct
{
    char call_to[DECODE_CALLSIGN_SIZE]; // call also be 'CQ'
    char call_from[DECODE_CALLSIGN_SIZE];
    char locator[DECODE_LOCATOR_SIZE]; // can also be a response 'RR73' etc.
    int freq_hz;
    int sync_score;
    int snr;
    int received_snr;
    char target_locator[DECODE_LOCATOR_SIZE];
    int slot;
    int RR73;
    Sequence sequence;
} Decode;

typedef struct
{
    char message[DISPLAY_MESSAGE_SIZE];
    int text_color;
} display_message;

typedef struct
{
    int number_times_called;
    char call[DECODE_CALLSIGN_SIZE];
    char locator[DECODE_LOCATOR_SIZE];
    int RSL;
    int received_RSL;
    int RR73;
    Sequence sequence;
} Calling_Station;

int Check_Calling_Stations(int num_decoded);
void display_messages(int decoded_messages);
void process_selected_Station(int num_decoded, int TouchIndex);
void clear_log_stored_data(void);
void set_QSO_Xmit_Freq(int freq);
void clear_decoded_messages(void);
void string_init(char *string, int size, uint8_t *is_initialised, char character);

#endif /* DECODE_FT8_H_ */
