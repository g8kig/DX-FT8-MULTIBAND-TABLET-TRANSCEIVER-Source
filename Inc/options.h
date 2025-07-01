/*
 * options.h
 *
 *  Created on: May 25, 2016
 *      Author: user
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_
#include <stdint.h>

typedef struct
{
	const char *Name;
	int16_t Initial;
	int16_t Minimum;
	const int16_t Maximum;
	const int16_t ChangeUnits;
	int16_t CurrentValue;
} OptionStruct;

typedef enum
{
	OPTION_Band_Index,
	OPTION_Logging_State,
	NUM_OPTIONS
} OptionNumber;

// Work with option data
int16_t Options_GetValue(int optionIdx);
void Options_SetValue(int optionIdx, int16_t newValue);
int16_t Read_Int_MicroSD(uint16_t MicroSD_Addr);
int16_t Write_Int_MicroSD(uint16_t MicroSD_Addr, int16_t value);

// MicroSD Access
int16_t Options_WriteToMicroSD(void);
int16_t Options_ReadFromMicroSD(void);
void Options_ResetToDefaults(void);
int16_t Options_Initialize(void);
int16_t Options_StoreValue(int optionIdx);
void Options_SetMinimum(int newMinimum);

#endif /* OPTIONS_H_ */
