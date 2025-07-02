/**
 ******************************************************************************
 * @brief   Main file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32f7xx_hal_tim.h"
#include "arm_math.h"

extern "C"
{
#include "constants.h"
}

#include "main.h"
#include "SDR_Audio.h"
#include "Display.h"
#include "Process_DSP.h"
#include "Codec_Gains.h"
#include "button.h"

#include "decode_ft8.h"
#include "gen_ft8.h"
#include "traffic_manager.h"
#include "button.h"
#include "DS3231.h"

#include "SiLabs.h"

#include "options.h"

uint32_t current_time, start_time, ft8_time;

int QSO_xmit;
int Xmit_DSP_counter;
int target_slot;
int target_freq;
int slot_state = 0;

static int master_decoded = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void CPU_CACHE_Enable(void);
static void InitialiseDisplay(void);
static bool Initialise_Serial();

static UART_HandleTypeDef s_UART1Handle = UART_HandleTypeDef();
static I2C_HandleTypeDef s_hI2C = I2C_HandleTypeDef();

static void update_synchronization(void)
{
	current_time = HAL_GetTick();
	ft8_time = current_time - start_time;

	if (ft8_time % 15000 <= 160 || FT_8_counter > 90)
	{
		ft8_flag = 1;
		FT_8_counter = 0;
		ft8_marker = 1;
		decode_flag = 0;

		if (QSO_xmit && target_slot == slot_state)
		{
			setup_to_transmit_on_next_DSP_Flag();
			update_log_display(1);
			QSO_xmit = 0;
		}
	}
}

int main(void)
{
	CPU_CACHE_Enable();

	HAL_Init();

	/* Configure the System clock to have a frequency of 200 MHz */
	SystemClock_Config();

	start_audio_I2C();

	PTT_Out_Init();

	Init_BoardVersionInput();
	Check_Board_Version();
	DeInit_BoardVersionInput();

	InitialiseDisplay();
	if (!Initialise_Serial())
	{
		Error_Handler();
	}

	HAL_Delay(10);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

	init_DSP();

	SD_Initialize();
	Read_Station_File();
	setup_display();

	Options_Initialize();

	EXT_I2C_Init();
	HAL_Delay(10);
	DS3231_init();
	display_Real_Date(0, 240);

	start_Si5351();

	Set_Cursor_Frequency();
	show_variable(400, 25, (int)NCO_Frequency);
	show_short(667, 255, AGC_Gain);
	start_duplex(0);
	HAL_Delay(10);
	set_codec_input_gain();
	HAL_Delay(10);
	receive_sequence();
	HAL_Delay(10);
	Set_Headphone_Gain(94);
	FT8_Sync();
	HAL_Delay(10);

	logger("Main loop starting", __FILE__, __LINE__);
	while (1)
	{
		if (DSP_Flag)
		{
			I2S2_RX_ProcessBuffer(buff_offset);

			if (xmit_flag)
			{
				// Start sending FT8 messages about 0.1 to 0.5 seconds into the time slot
				// to match the observed behavior.
				if (ft8_xmit_delay >= 28)
				{
					if (!Tune_On)
					{
						if ((ft8_xmit_counter < 79) && (Xmit_DSP_counter % 4 == 0))
						{
							set_FT8_Tone(tones[ft8_xmit_counter]);
							ft8_xmit_counter++;
						}

						Xmit_DSP_counter++;

						if (ft8_xmit_counter == 79)
						{
							xmit_flag = 0;
							ft8_receive_sequence();
							receive_sequence();
							ft8_xmit_delay = 0;
							if (!Beacon_On)
								clear_queued_message();
						}
					}
				}
				else
				{
					if (++ft8_xmit_delay == 16)
						output_enable(SI5351_CLK0, 1);
				}
			}

			display_RealTime(100, 240);

			if (Tune_On)
			{
				display_Real_Date(0, 240);
			}

			DSP_Flag = 0;
		}

		if (decode_flag && !Tune_On && !xmit_flag)
		{
			// toggle the slot state
			slot_state = (slot_state == 0) ? 1 : 0;
			clear_decoded_messages();

			master_decoded = ft8_decode();
			if (master_decoded > 0)
			{
				display_messages(master_decoded);
				if (Beacon_On)
					service_Beacon_mode(master_decoded);
				else
					service_QSO_mode(master_decoded);
			}

			decode_flag = 0;
		} // end of servicing FT_Decode

		if (FT_8_counter > 0 && FT_8_counter < 90)
			Process_Touch();

		if (!Tune_On && FT8_Touch_Flag && !Beacon_On)
			process_selected_Station(master_decoded, FT_8_TouchIndex);

		update_synchronization();
	}
}

/**
 * @brief  Initialise Display
 * @param  None
 * @retval None
 */
static void InitialiseDisplay(void)
{
	/* Configure Key button */
	BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_GPIO);

	/* Configure LED1 */
	BSP_LED_Init(LED1);

	/* Initialize the LCD */
	BSP_LCD_Init();

	/* LCD Layer Initialization */
	BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

	/* Select the LCD Layer */
	BSP_LCD_SelectLayer(1);

	/* Enable the display */
	BSP_LCD_DisplayOn();
}

/**
 * @brief This function provides accurate delay (in milliseconds) based
 *        on SysTick counter flag.
 * @note This function is declared as __weak to be overwritten in case of other
 *       implementations in user file.
 * @param Delay: specifies the delay time length, in milliseconds.
 * @retval None
 */

void HAL_Delay(__IO uint32_t Delay)
{
	while (Delay != 0)
	{
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			Delay--;
		}
	}
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 200000000
 *            HCLK(Hz)                       = 200000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 400
 *            PLL_P                          = 2
 *            PLLSAI_N                       = 384
 *            PLLSAI_P                       = 8
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/* Activate the OverDrive to reach the 216 Mhz Frequency */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler();
	}

	/* Select PLLSAI output as USB clock source */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 4;
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
	/* User may add here some code to deal with this error */
	while (1)
	{
	}
}

/**
 * @brief  CPU L1-Cache enable.
 * @param  None
 * @retval None
 */
static void CPU_CACHE_Enable(void)
{
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

static bool Initialise_Serial()
{
	__USART1_CLK_ENABLE();
	__I2C1_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStructure;

	// Serial debug Port USART1 TX/RX : PA9/PB7
	GPIO_InitStructure.Pin = GPIO_PIN_9; // TX
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_7; // RX
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	// I2C1 SCL/SDA : PB8/PB9
	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	s_UART1Handle.Instance = USART1;
	s_UART1Handle.Init.BaudRate = 115200;
	s_UART1Handle.Init.WordLength = UART_WORDLENGTH_8B;
	s_UART1Handle.Init.StopBits = UART_STOPBITS_1;
	s_UART1Handle.Init.Parity = UART_PARITY_NONE;
	s_UART1Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	s_UART1Handle.Init.Mode = UART_MODE_TX_RX;

	s_hI2C.Instance = I2C1;
	s_hI2C.Init.Timing = 0x00100413;
	s_hI2C.Init.OwnAddress1 = 0;
	s_hI2C.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	s_hI2C.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	s_hI2C.Init.OwnAddress2 = 0;
	s_hI2C.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	s_hI2C.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	s_hI2C.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	return ((HAL_I2C_Init(&s_hI2C) == HAL_OK) && 
			(HAL_UART_Init(&s_UART1Handle) == HAL_OK));
}

void logger(const char *message, const char *file, int line)
{
	char buffer[256];
	if (snprintf(buffer, sizeof(buffer), "%s:%d: %s\n", file, line, message) > 0)
	{
		HAL_UART_Transmit(&s_UART1Handle, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
	}
}

/************************ Portions (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
