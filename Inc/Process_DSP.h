/*
 * Process_DSP.h
 *
 *  Created on: Feb 6, 2016
 *      Author: user
 */

#ifndef PROCESS_DSP_H_
#define PROCESS_DSP_H_

#include "SDR_Audio.h"
#include "arm_math.h"

#include "constants.h"

void Process_FIR_I_32K(void);
void Process_FIR_Q_32K(void);

void init_DSP(void);

#define ft8_buffer_size 400 //arbitrary for 3 kHz

#define ft8_min_bin 48

#define ft8_min_freq FFT_Resolution * ft8_min_bin
#define ft8_msg_samples 91

#define FFT_SIZE  2048
#define input_gulp_size 1024

extern double NCO_Frequency;

extern int ft8_flag, FT_8_counter, ft8_marker;
extern uint8_t FFT_Buffer[ft8_buffer_size];

extern uint8_t export_fft_power[ft8_msg_samples * ft8_buffer_size * 4];

int ft8_decode(void);
void process_FT8_FFT(void);

#endif /* PROCESS_DSP_H_ */
