/*
 * gen_ft8.h
 *
 *  Created on: Oct 30, 2019
 *      Author: user
 */

#ifndef GEN_FT8_H_
#define GEN_FT8_H_

#include <math.h>
#include "arm_math.h"

extern char Target_Frequency[8]; // Seven character frequency  + /0
extern char Locator[5]; // four character locator  + /0
extern char Station_Call[7]; //six character call sign + /0
extern char Target_Call[7]; //six character call sign + /0
extern char Target_Locator[5]; // four character locator  + /0
extern int Target_RSL; // four character RSL  + /0
extern char CQ_Target_Call[7];

extern char reply_message[21];
extern char reply_message_list[18][8];
extern int reply_message_count;

extern char SDPath[4]; /* SD card logical drive path */

void clear_reply_message_box(void);
void set_reply(uint16_t index);
void set_cq(void);
void set_CQ_reply(void);

void Open_Station_File(void);
void Write_Station_File(void);
void Read_Station_File(void);
void Station_Data_Initialize(void);
void SD_Initialize(void);

void set_CQ_message(uint16_t index);
void compose_messages(void);
void clear_xmit_messages(void);
void que_message(int index);
void clear_qued_message(void);

#endif /* GEN_FT8_H_ */
