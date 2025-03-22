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

extern char Station_Call[10];  // seven character call sign (e.g. 3DA0XYZ) + optional /P + null terminator
extern char Locator[5];        // four character locator  + /0
extern char Target_Call[10];   // same as Station_Call
extern char Target_Locator[5]; // same as Locator
extern int Target_RSL;

#define MESSAGE_SIZE 26

extern char reply_message[MESSAGE_SIZE];
extern char reply_message_list[MESSAGE_SIZE][8];
extern int reply_message_count;

extern char SDPath[4]; /* SD card logical drive path */

extern int CQ_Mode_Index;
extern int Free_Index;

void clear_reply_message_box(void);

enum
{
    Reply_RSL = 0,
    Reply_Beacon_73,
    Reply_R_RSL,
    Reply_QSO_73
};

void set_reply(uint16_t index);
void set_cq(void);

void Read_Station_File(void);
void SD_Initialize(void);

void compose_messages(void);
void clear_xmit_messages(void);
void que_message(int index);
void clear_qued_message(void);

#endif /* GEN_FT8_H_ */
