# ---------------------------
# STM_STD_LIB
# ---------------------------

INCLUDES_STD_LIB = \
-I "${GCC_PLACE}arm-none-eabi/include" \
-I "system/include" \
-I "system/include/cmsis" \
-I "system/include/${STM32_PERIPH_DIR}/stm32f3-stdperiph" \
-I "system/include/additionally"

# ---------------------------
# Исходные файлы
# ---------------------------

# Исходные файлы stm32 periph
STM32_PERIPH_DIR = system/src/stm32f3-stdperiph

STM32_PERIPH_SRC = \
${STM32_PERIPH_DIR}/stm32f30x_adc.h \
${STM32_PERIPH_DIR}/stm32f30x_can.h \
${STM32_PERIPH_DIR}/stm32f30x_comp.h \
${STM32_PERIPH_DIR}/stm32f30x_crc.h \
${STM32_PERIPH_DIR}/stm32f30x_dac.h \
${STM32_PERIPH_DIR}/stm32f30x_dbgmcu.h \
${STM32_PERIPH_DIR}/stm32f30x_dma.h \
${STM32_PERIPH_DIR}/stm32f30x_exti.h \
${STM32_PERIPH_DIR}/stm32f30x_flash.h \
${STM32_PERIPH_DIR}/stm32f30x_gpio.h \
${STM32_PERIPH_DIR}/stm32f30x_i2c.h \
${STM32_PERIPH_DIR}/stm32f30x_it.h \
${STM32_PERIPH_DIR}/stm32f30x_iwdg.h \
${STM32_PERIPH_DIR}/stm32f30x_misc.h \
${STM32_PERIPH_DIR}/stm32f30x_opamp.h \
${STM32_PERIPH_DIR}/stm32f30x_pwr.h \
${STM32_PERIPH_DIR}/stm32f30x_rcc.h \
${STM32_PERIPH_DIR}/stm32f30x_rtc.h \
${STM32_PERIPH_DIR}/stm32f30x_spi.h \
${STM32_PERIPH_DIR}/stm32f30x_syscfg.h \
${STM32_PERIPH_DIR}/stm32f30x_tim.h \
${STM32_PERIPH_DIR}/stm32f30x_usart.h \
${STM32_PERIPH_DIR}/stm32f30x_wwdg.h \
${STM32_PERIPH_DIR}/stm32f3_discovery.h \
${STM32_PERIPH_DIR}/stm32f3_discovery_l3gd20.h \
${STM32_PERIPH_DIR}/stm32f3_discovery_lsm303dlhc.h

# Исходные файлы cmsis
CMSIS_DIR = system/src/cmsis

CMSIS_SRC = \


# ---------------------------
STM32_STD_LIB: 
	@echo "STM32_STD_LIB"

