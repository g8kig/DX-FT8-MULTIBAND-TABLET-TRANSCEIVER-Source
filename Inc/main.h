
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"

#include "stm32746g_discovery.h"
#include "lcd_log.h"
#include "stm32f7xx_hal.h"

void HAL_Delay(uint32_t Delay);

#define NoOp  __NOP()

extern uint32_t current_time, start_time, ft8_time;
/*
extern uint8_t ft8_hours, ft8_minutes, ft8_seconds;
extern uint32_t hours_fraction;
extern uint32_t minute_fraction;
*/
extern int QSO_xmit;
extern int Xmit_DSP_counter;

extern int slot_state;
extern int target_slot;
extern int target_freq;

void update_synchronization(void);
void update_slot_status(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
