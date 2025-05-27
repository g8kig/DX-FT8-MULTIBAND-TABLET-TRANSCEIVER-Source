/*
 * log_file.h
 *
 *  Created on: Oct 29, 2019
 *      Author: user
 */

#ifndef LOG_FILE_H_
#define LOG_FILE_H_

#include "arm_math.h"

#define RTC_STRING_SIZE 13
#define FILENAME_STRING_SIZE 24

void Write_Log_Data(const char *file_data);
void Close_Log_File(void);
void Open_Log_File();
void Init_Log_File(void);

#endif /* LOG_FILE_H_ */
