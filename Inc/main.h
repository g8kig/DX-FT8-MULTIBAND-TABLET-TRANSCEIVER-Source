
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#define NoOp  __NOP()

extern uint32_t start_time, ft8_time;

extern int QSO_xmit;
extern int Xmit_DSP_counter;

extern int slot_state;
extern int target_slot;
extern int target_freq;

void logger(const char *message, const char* file, int line);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
