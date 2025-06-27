/*
 * SDR_Audio.c
 *
 *  Created on: May 10, 2016
 *      Author: user
 */

#include "SDR_Audio.h"
#include "stm32746g_discovery_audio.h"
#include "wm8994.h"
#include "stm32746g_discovery_lcd.h"
#include "arm_math.h"
#include "Process_DSP.h"
#include <stdlib.h>

#include "button.h"
#include "Sine_table.h"

q15_t FIR_I_In[BUFFERSIZE / 4];
q15_t FIR_Q_In[BUFFERSIZE / 4];
q15_t FIR_I_Out[BUFFERSIZE / 4];
q15_t FIR_Q_Out[BUFFERSIZE / 4];
q15_t USB_Out[BUFFERSIZE / 4];
q15_t LSB_Out[BUFFERSIZE / 4];
q15_t in_buff[BUFFERSIZE];

q15_t FT8_Data[2048 / 2];
q15_t out_buff[BUFFERSIZE];

uint16_t buff_offset;

float x_NCOphzinc;

int DSP_Flag;
int Xmit_Mode;
int xmit_flag, ft8_xmit_counter, ft8_xmit_flag, ft8_xmit_delay;

#define PI2 6.2831853071795864765
#define KCONV 10430.37835 // 		4096*16/PI2

const double LO_Freq = 10000;
const float Sample_Frequency = 32000.0;

void start_audio_I2C(void)
{
	AUDIO_IO_Init();
}

void start_codec(void)
{
	wm8994_Reset(AUDIO_I2C_ADDRESS);
	HAL_Delay(100);
	wm8994_Init(AUDIO_I2C_ADDRESS,
				INPUT_DEVICE_INPUT_LINE_1 | OUTPUT_DEVICE_BOTH, 70,
				(uint32_t)16000);
	HAL_Delay(100);
}

void start_duplex(int mode)
{
	memset(out_buff, 0, sizeof(out_buff));
	HAL_Delay(10);
	if (mode == 0)
		BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_BOTH, 70,
							  32000);
	else
		BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_BOTH, 70,
							  8000);

	BSP_AUDIO_IN_Record((uint16_t *)&in_buff, BUFFERSIZE);
	BSP_AUDIO_OUT_Play((uint16_t *)&out_buff, 2 * BUFFERSIZE);

	NoOp;
}

void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
	buff_offset = BUFFERSIZE / 2;
	DSP_Flag = 1;
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
	buff_offset = 0;
	DSP_Flag = 1;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
}

static int frame_counter = 0;
const float m_RMSConstant = 1.0 / 65485.0;

void I2S2_RX_ProcessBuffer(uint16_t offset)
{
	static q15_t TX_I;
	static long NCO_phz;

	x_NCOphzinc = (PI2 * LO_Freq / (double)Sample_Frequency);

	for (int i = 0; i < BUFFERSIZE / 4; i++)
	{
		NCO_phz += (long)(KCONV * (x_NCOphzinc));
		TX_I = (Sine_table[(NCO_phz >> 4) & 0xFFF]);
		
		FIR_I_In[i] = (q15_t)(TX_I * in_buff[i * 2 + offset] * m_RMSConstant);
		FIR_Q_In[i] = (q15_t)(TX_I * in_buff[i * 2 + 1 + offset] * m_RMSConstant);
	}

	Process_FIR_I_32K();
	Process_FIR_Q_32K();

	for (int i = 0; i < BUFFERSIZE / 4; i++)
	{
		USB_Out[i] = FIR_I_Out[i] - FIR_Q_Out[i];
		LSB_Out[i] = FIR_I_Out[i] + FIR_Q_Out[i];

		if (frame_counter < 4)
		{
			if (i % 5 == 0)
				FT8_Data[i / 5 + frame_counter * 256] = USB_Out[i];
		}
		
		out_buff[i * 2 + offset] = USB_Out[i];
		out_buff[i * 2 + 1 + offset] = LSB_Out[i];
	}

	if (++frame_counter == 4)
	{
		process_FT8_FFT();
		frame_counter = 0;
	}
}
