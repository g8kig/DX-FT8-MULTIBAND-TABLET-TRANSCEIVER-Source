/*
 * defines.h
 *
 *  Created on: Nov 1, 2024
 *      Author: Paul
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define EXTRA_SPACE_SIZE        1
#define SD_PATH_SIZE            4
#define LOCATOR_SIZE            5
#define REPORT_SIZE             5
#define CALL_SIZE               7
#define FIELD3_SIZE             7
#define FREQUENCY_SIZE          8
#define DATE_STRING_SIZE		9
#define TIME_STRING_SIZE		9
#define DECODE_TIME_SIZE	   10
#define FIELD1_SIZE			   14
#define FIELD2_SIZE			   14
#define MESSAGE_SIZE           FIELD1_SIZE+FIELD2_SIZE+FIELD3_SIZE+EXTRA_SPACE_SIZE
#define BUFFER_SIZE           128
#define LOG_LINE_SIZE		  180

#define XMIT_MESSAGE_COUNT      3
#define LOG_MESSAGE_COUNT       8
#define BEACON_MESSAGE_COUNT   10
#define DISPLAY_MESSAGE_COUNT  10
#define DECODED_MESSAGE_COUNT  10
#define CANDIDATE_COUNT		   20
#define ANSWER_CQ_COUNT		   50

#define NUM_BANDS               5
#define NUM_BUTTONS            28
#endif // DEFINES_H
