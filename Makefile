# STM32 toolchain path
TOOLCHAIN_PATH = $(HOME)/.platformio/packages/toolchain-gccarmnoneeabi/bin/
CC = $(TOOLCHAIN_PATH)/arm-none-eabi-gcc
SIZE = $(TOOLCHAIN_PATH)/arm-none-eabi-size
OBJDUMP = $(TOOLCHAIN_PATH)/arm-none-eabi-objdump
OBJCOPY = $(TOOLCHAIN_PATH)/arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F746xx -DUSE_STM32746G_DISCO -DUSE_IOEXPANDER \
         -IInc -IDrivers/CMSIS/Include -IDrivers/CMSIS/Device/ST/STM32F7xx/Include \
         -IDrivers/STM32F7xx_HAL_Driver/Inc -IDrivers/BSP/STM32746G-Discovery -IDrivers/BSP/Common \
         -IUtilities/Fonts -Os -ffunction-sections --specs=nano.specs -mfpu=fpv5-sp-d16 \
         -mfloat-abi=hard -mthumb -Wall -Wextra

EXTRA_INCLUDES = \
    -IDrivers/BSP/STM32746G-Discovery \
    -IDrivers/BSP/STM32746G_DISCOVERY \
    -IDrivers/BSP/Common \
    -IDrivers/BSP/exc7200 \
    -IDrivers/BSP/ft5336 \
    -IDrivers/BSP/mfxstm32l152 \
    -IDrivers/BSP/ov9655 \
    -IDrivers/BSP/s5k5cag \
    -IDrivers/BSP/st7735 \
    -IDrivers/BSP/stmpe811 \
    -IDrivers/BSP/ts3510 \
    -IDrivers/BSP/wm8994 \
    -IDrivers/BSP/rk043fn48h \
	-IFT8_library \
	-IMiddlewares/src 

ASMFLAGS = -mcpu=cortex-m7 -g3 -c -Wall -Wextra -x assembler-with-cpp --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb

LDFLAGS = -mcpu=cortex-m7 -TSTM32F746NGHx_FLASH.ld --specs=nosys.specs -Wl,-Map=Katy.map -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group

ASM_SRCS = Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s DSP_CMSIS/arm_bitreversal2.s

C_SRCS = \
Drivers/BSP/exc7200/exc7200.c \
Drivers/BSP/ft5336/ft5336.c \
Drivers/BSP/mfxstm32l152/mfxstm32l152.c \
Drivers/BSP/ov9655/ov9655.c \
Drivers/BSP/s5k5cag/s5k5cag.c \
Drivers/BSP/st7735/st7735.c \
Drivers/BSP/stmpe811/stmpe811.c \
Drivers/BSP/ts3510/ts3510.c \
Drivers/BSP/wm8994/wm8994.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_audio.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_eeprom.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_lcd.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sd.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_sdram.c \
Drivers/BSP/STM32746G_DISCOVERY/stm32746g_discovery_ts.c \
Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_adc_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_can.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cec.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_crc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_crc_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cryp.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cryp_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dac.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dac_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dcmi.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dcmi_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma2d.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_eth.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_hash.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_hash_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_hcd.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2s.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_irda.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_iwdg.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_lptim.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_ltdc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_msp_template.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_nand.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_nor.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_qspi.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rng.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rtc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rtc_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sai.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sai_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sd.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sdram.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_smartcard.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_smartcard_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spdifrx.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spi.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_sram.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_usart.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_wwdg.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_fmc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_sdmmc.c \
Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_usb.c \
DSP_CMSIS/arm_bitreversal.c \
DSP_CMSIS/arm_cfft_q15.c \
DSP_CMSIS/arm_cfft_radix2_init_q15.c \
DSP_CMSIS/arm_cfft_radix2_q15.c \
DSP_CMSIS/arm_cfft_radix4_init_q15.c \
DSP_CMSIS/arm_cfft_radix4_q15.c \
DSP_CMSIS/arm_cmplx_mag_squared_q15.c \
DSP_CMSIS/arm_common_tables.c \
DSP_CMSIS/arm_const_structs.c \
DSP_CMSIS/arm_fir_decimate_q15.c \
DSP_CMSIS/arm_fir_q15.c \
DSP_CMSIS/arm_rfft_init_q15.c \
DSP_CMSIS/arm_rfft_q15.c \
DSP_CMSIS/arm_scale_q15.c \
DSP_CMSIS/arm_shift_q15.c \
FT8_library/constants.c \
FT8_library/decode.c \
FT8_library/encode.c \
FT8_library/ldpc.c \
FT8_library/pack.c \
FT8_library/text.c \
FT8_library/unpack.c \
Middlewares/src/ccsbcs.c \
Middlewares/src/diskio.c \
Middlewares/src/ff.c \
Middlewares/src/ff_gen_drv.c \
Middlewares/src/sdram_diskio.c \
Middlewares/src/sd_diskio.c \
Middlewares/src/unicode.c \
Middlewares/src/option/syscall.c \
Src/ADIF.c \
Src/button.c \
Src/Codec_Gains.c \
Src/decode_ft8.c \
Src/Display.c \
Src/DS3231.c \
Src/FIR_Coefficients.c \
Src/gen_ft8.c \
Src/log_file.c \
Src/main.c \
Src/options.c \
Src/Process_DSP.c \
Src/SDR_Audio.c \
Src/SiLabs.c \
Src/Sine_table.c \
Src/stm32f7xx_it.c \
Src/traffic_manager.c \
Src/Ini.c

# Object files
ASM_OBJS = $(ASM_SRCS:.s=.o)
C_OBJS = $(C_SRCS:.c=.o)
OBJS = $(ASM_OBJS) $(C_OBJS)

TARGET = Katy.elf

.PHONY: all clean

all: $(TARGET) Katy.hex Katy.list

#$(C_OBJS) : $(C_SRCS)
#    $(CC) $(CFLAGS) -c $(C_SRCS)

%.o: %.c
	 $(CC) $(CFLAGS) $(EXTRA_INCLUDES) -c $< -o $@

%.o: %.s
	$(CC) $(ASMFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

Katy.hex: $(TARGET)
	$(OBJCOPY) -O ihex $(TARGET) $@

Katy.list: $(TARGET)
	$(OBJDUMP) -h -S $(TARGET) > $@

size: $(TARGET)
	$(SIZE) $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) Katy.hex Katy.list
