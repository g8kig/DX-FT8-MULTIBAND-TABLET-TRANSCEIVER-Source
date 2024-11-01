/*
 * SDR_Audio.h
 *
 *  Created on: May 10, 2016
 *      Author: user
 */

#ifndef SDR_AUDIO_H_
#define SDR_AUDIO_H_
#include "arm_math.h"

#define BUFFERSIZE 1280 * 4

#define NoOp  __NOP()

extern q15_t FIR_I_In[BUFFERSIZE / 4];
extern q15_t FIR_Q_In[BUFFERSIZE / 4];
extern q15_t FIR_I_Out[BUFFERSIZE / 4];
extern q15_t FIR_Q_Out[BUFFERSIZE / 4];
extern q15_t USB_Out[BUFFERSIZE / 4];
extern q15_t LSB_Out[BUFFERSIZE / 4];
extern q15_t in_buff[BUFFERSIZE];

extern q15_t FT8_Data[2048 / 2];
extern q15_t out_buff[BUFFERSIZE];

extern uint16_t buff_offset;

//extern float R_lgain;
extern float x_NCOphzinc;

extern int DSP_Flag;
extern int Xmit_Mode;
extern int xmit_flag, ft8_xmit_counter, ft8_xmit_flag, ft8_xmit_delay;

extern double ft8_shift;

#define ft8_hz 6.25

void transfer_buffers(void);
void start_duplex(int mode);
void start_audio_I2C(void);
void start_codec(void);

void I2S2_RX_ProcessBuffer(uint16_t offset);
void I2S2_FT8_Xmit_ProcessBuffer(uint16_t offset);

#endif /* SDR_AUDIO_H_ */
