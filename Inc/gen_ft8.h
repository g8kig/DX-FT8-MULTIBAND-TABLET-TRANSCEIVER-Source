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

#define CALLSIGN_SIZE 10
#define LOCATOR_SIZE 5

extern char Station_Call[CALLSIGN_SIZE];  // seven character call sign (e.g. 3DA0XYZ) + optional /P + null terminator
extern char Locator[LOCATOR_SIZE];        // four character locator  + /0
extern char Target_Call[CALLSIGN_SIZE];   // same as Station_Call
extern char Target_Locator[LOCATOR_SIZE]; // same as Locator
extern int Target_RSL;

#define REPLY_MESSAGE_SIZE 28

extern char reply_message[REPLY_MESSAGE_SIZE];

extern char SDPath[4]; /* SD card logical drive path */

extern int CQ_Mode_Index;
extern int Free_Index;

void clear_reply_message_box(void);

typedef enum _ReplyID
{
    Reply_RSL,
    Reply_R_RSL,
    Reply_Beacon_73,
    Reply_QSO_73
} ReplyID;

typedef enum _QueID
{
    Que_Locator,
    Que_RSL,
    Que_73,
    Que_Size
} QueID;

void set_reply(ReplyID index);
void set_cq(void);

void Read_Station_File(void);
void SD_Initialize(void);

void compose_messages(void);
void clear_xmit_messages(void);
void queue_message(QueID queId);
void clear_queued_message(void);

#endif /* GEN_FT8_H_ */
