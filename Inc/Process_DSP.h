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

#define ft8_buffer 400 //arbitrary for 3 kc

#define ft8_min_bin 48

#define ft8_min_freq FFT_Resolution * ft8_min_bin
#define ft8_msg_samples 91

#define FFT_SIZE  2048
#define input_gulp_size 1024

extern double NCO_Frequency;

extern int ft8_flag, FT_8_counter, ft8_marker;
extern q15_t window_dsp_buffer[FFT_SIZE];
extern uint16_t FFT_Buffer[2 * ft8_buffer];

extern q15_t extract_signal[input_gulp_size * 3];  // was float
extern q15_t dsp_output[FFT_SIZE * 2];

extern uint8_t export_fft_power[ft8_msg_samples * ft8_buffer * 4];

extern int offset_step;

extern q15_t FFT_Scale[FFT_SIZE * 2];
extern q15_t FFT_Magnitude[FFT_SIZE];
extern int32_t FFT_Mag_10[FFT_SIZE / 2];

extern float mag_db[FFT_SIZE / 2 + 1];
extern float window[FFT_SIZE];

//extern int RSL;
//extern int Gadc;
//extern int Gdac;

void Process_FIR_I_32K(void);
void Process_FIR_Q_32K(void);

void process_FT8_FFT(void);
int ft8_decode(void);

void extract_power(int offset);
#endif /* PROCESS_DSP_H_ */
