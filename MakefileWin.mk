# куда кладем результат сборки проекта и где будут объектные файлы
BIN_PLACE = Debug_Win/

PROGRAM_NAME = STM_ACC_GIRO_FILTERED

# как будет называться исполняемый файл - итог сборки
BINARY = ${PROGRAM_NAME}.elf

#GCC_PLACE=/home/mike/gcc-arm-none-eabi-8-2019-q3-update/
GCC_PLACE="c:/SysGCC/arm-eabi/"
#исполняемый файл компиллятора-компоновщика
CC = ${GCC_PLACE}bin/arm-none-eabi-gcc
CP = ${GCC_PLACE}bin/arm-none-eabi-g++

#включаемые директории
INCLUDES = \
-I"${GCC_PLACE}arm-none-eabi/include" \
-I"include" \
-I"src" \
-I"system/include" \
-I"system/include/cmsis" \
-I"system/include/stm32f3-stdperiph" \
-I"system/include/additionally" \
-I"system/USB_LIB/include"

# общие флаги компиллятора
# COMPILER_FLAGS = \
# -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char \
# -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra -g3
COMPILER_FLAGS = \
-mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char \
-ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra -g3

# предопределенные константы
DEFINES= \
-DUSE_STDPERIPH_DRIVER \
-DDEBUG \
-DSTM32F30X \
-DHSE_VALUE=8000000 \

# флаги для gcc  - не перепутывать!!! только gnu11 разрешает пользоваться
# директивами встроенного ассемблера "asm" -они могут встречаться в стандартной библиотеке

GCC_FLAGS = -std=gnu11 ${COMPILER_FLAGS} -c

# флаги для g++
# GPP_FLAGS = -std=gnu++11 ${COMPILER_FLAGS} -c -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics
GPP_FLAGS = -std=gnu++11 ${COMPILER_FLAGS} -c -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics

# флаги для линковщика
LINK_FLAGS = ${COMPILER_FLAGS} \
-T "ldscripts/mem.ld" \
-T "ldscripts/libs.ld" \
-T "ldscripts/sections.ld" \
-L"${PROJ}ldscripts" \
-nostartfiles -Xlinker --gc-sections -Wl,-Map,${BIN_PLACE}${PROGRAM_NAME}.map # --specs=nano.specs 

# библиотеки компоновщика - должны быть последними в списке команды компоновщика
# C:\SysGCC\arm-eabi\arm-none-eabi\lib\libm.a
LIBS = -L${GCC_PLACE}arm-none-eabi/lib -lm


OBJECTS = \
${BIN_PLACE}stm32f30x/stm32f30x_usart.o \
${BIN_PLACE}stm32f30x/stm32f30x_tim.o \
${BIN_PLACE}stm32f30x/stm32f30x_rcc.o  \
${BIN_PLACE}stm32f30x/stm32f30x_misc.o \
${BIN_PLACE}stm32f30x/stm32f30x_gpio.o  \
${BIN_PLACE}stm32f30x/stm32f30x_it.o  \
${BIN_PLACE}stm32f30x/stm32f30x_i2c.o  \
${BIN_PLACE}stm32f30x/stm32f30x_exti.o  \
${BIN_PLACE}stm32f30x/stm32f30x_spi.o  \
${BIN_PLACE}stm32f30x/stm32f30x_syscfg.o  \
${BIN_PLACE}stm32f3/stm32f3_discovery.o \
${BIN_PLACE}stm32f3/stm32f3_discovery_lsm303dlhc.o  \
${BIN_PLACE}stm32f3/stm32f3_discovery_l3gd20.o \
${BIN_PLACE}system/system_stm32f30x.o \
${BIN_PLACE}system/vectors_stm32f30x.o  \
${BIN_PLACE}system/_cxx.opp \
${BIN_PLACE}system/_exit.o \
${BIN_PLACE}system/_sbrk.o \
${BIN_PLACE}system/_startup.o \
${BIN_PLACE}system/_syscalls.o \
${BIN_PLACE}system/assert.o \
${BIN_PLACE}system/_initialize_hardware.o \
${BIN_PLACE}system/_reset_hardware.o \
${BIN_PLACE}system/exception_handlers.o \
${BIN_PLACE}system/hw_config.o \
${BIN_PLACE}usb/usb_istr.o \
${BIN_PLACE}usb/usb_pwr.o \
${BIN_PLACE}usb/usb_init.o \
${BIN_PLACE}usb/usb_prop.o \
${BIN_PLACE}usb/usb_regs.o \
${BIN_PLACE}usb/usb_desc.o \
${BIN_PLACE}usb/usb_sil.o \
${BIN_PLACE}usb/usb_core.o \
${BIN_PLACE}usb/usb_int.o \
${BIN_PLACE}usb/usb_mem.o \
${BIN_PLACE}usb/usb_endp.o \
${BIN_PLACE}user/Drv_Gpio.opp \
${BIN_PLACE}user/Drv_Uart.opp \
${BIN_PLACE}user/main.opp \
${BIN_PLACE}user/Sensors.o \
${BIN_PLACE}user/COM_IO.opp 

${BIN_PLACE}$(BINARY) : $(OBJECTS)
	echo "BUILD "${BIN_PLACE}${BINARY}", MEMORY CARD "${BIN_PLACE}${PROGRAM_NAME}.map
	$(CC) ${LINK_FLAGS} -o ${BIN_PLACE}$(BINARY) $(OBJECTS) ${LIBS}
	${GCC_PLACE}bin/arm-none-eabi-size --format=berkeley ${BIN_PLACE}${BINARY}
	@echo "FORMING "${BIN_PLACE}${PROGRAM_NAME}.hex 
	${GCC_PLACE}bin/arm-none-eabi-objcopy -O ihex ${BIN_PLACE}${BINARY} ${BIN_PLACE}${PROGRAM_NAME}.hex 

%.o:
	echo "gcc "$<
	$(CC) ${GCC_FLAGS} ${DEFINES} ${INCLUDES}  $< -o $@

%.opp:
	echo "g++ "$<
	$(CP) ${GPP_FLAGS} ${DEFINES} ${INCLUDES} $< -o $@


# stm32f30x
${BIN_PLACE}stm32f30x/stm32f30x_usart.o : system/src/stm32f3-stdperiph/stm32f30x_usart.c
${BIN_PLACE}stm32f30x/stm32f30x_tim.o : system/src/stm32f3-stdperiph/stm32f30x_tim.c
${BIN_PLACE}stm32f30x/stm32f30x_rcc.o : system/src/stm32f3-stdperiph/stm32f30x_rcc.c
${BIN_PLACE}stm32f30x/stm32f30x_misc.o : system/src/stm32f3-stdperiph/stm32f30x_misc.c
${BIN_PLACE}stm32f30x/stm32f30x_gpio.o : system/src/stm32f3-stdperiph/stm32f30x_gpio.c
${BIN_PLACE}stm32f30x/stm32f30x_it.o : system/src/stm32f3-stdperiph/stm32f30x_it.c
${BIN_PLACE}stm32f30x/stm32f30x_i2c.o : system/src/stm32f3-stdperiph/stm32f30x_i2c.c
${BIN_PLACE}stm32f30x/stm32f30x_exti.o : system/src/stm32f3-stdperiph/stm32f30x_exti.c
${BIN_PLACE}stm32f30x/stm32f30x_spi.o : system/src/stm32f3-stdperiph/stm32f30x_spi.c
${BIN_PLACE}stm32f30x/stm32f30x_syscfg.o : system/src/stm32f3-stdperiph/stm32f30x_syscfg.c

# stm32f3
${BIN_PLACE}stm32f3/stm32f3_discovery.o : system/src/stm32f3-stdperiph/stm32f3_discovery.c
${BIN_PLACE}stm32f3/stm32f3_discovery_lsm303dlhc.o : system/src/stm32f3-stdperiph/stm32f3_discovery_lsm303dlhc.c
${BIN_PLACE}stm32f3/stm32f3_discovery_l3gd20.o : system/src/stm32f3-stdperiph/stm32f3_discovery_l3gd20.c

# system
${BIN_PLACE}system/system_stm32f30x.o : system/src/cmsis/system_stm32f30x.c
${BIN_PLACE}system/vectors_stm32f30x.o  : system/src/cmsis/vectors_stm32f30x.c
${BIN_PLACE}system/_cxx.opp : system/src/newlib/_cxx.cpp
${BIN_PLACE}system/_exit.o : system/src/newlib/_exit.c
${BIN_PLACE}system/_sbrk.o : system/src/newlib/_sbrk.c
${BIN_PLACE}system/_startup.o : system/src/newlib/_startup.c
${BIN_PLACE}system/_syscalls.o : system/src/newlib/_syscalls.c
${BIN_PLACE}system/assert.o  : system/src/newlib/assert.c
${BIN_PLACE}system/_initialize_hardware.o : system/src/cortexm/_initialize_hardware.c
${BIN_PLACE}system/_reset_hardware.o : system/src/cortexm/_reset_hardware.c
${BIN_PLACE}system/exception_handlers.o : system/src/cortexm/exception_handlers.c
${BIN_PLACE}system/hw_config.o : system/src/stm32f3-stdperiph\hw_config.c

# usb
${BIN_PLACE}usb/usb_istr.o : system/USB_LIB/src/usb_istr.c
${BIN_PLACE}usb/usb_pwr.o : system/USB_LIB/src/usb_pwr.c
${BIN_PLACE}usb/usb_init.o : system/USB_LIB/src/usb_init.c
${BIN_PLACE}usb/usb_prop.o : system/USB_LIB/src/usb_prop.c
${BIN_PLACE}usb/usb_regs.o : system/USB_LIB/src/usb_regs.c
${BIN_PLACE}usb/usb_desc.o : system/USB_LIB/src/usb_desc.c
${BIN_PLACE}usb/usb_sil.o : system/USB_LIB/src/usb_sil.c
${BIN_PLACE}usb/usb_core.o : system/USB_LIB/src/usb_core.c
${BIN_PLACE}usb/usb_int.o : system/USB_LIB/src/usb_int.c
${BIN_PLACE}usb/usb_mem.o : system/USB_LIB/src/usb_mem.c
${BIN_PLACE}usb/usb_endp.o : system/USB_LIB/src/usb_endp.c

# user
${BIN_PLACE}user/Drv_Gpio.opp : src/Drv_Gpio.cpp
${BIN_PLACE}user/Drv_Uart.opp : src/Drv_Uart.cpp
${BIN_PLACE}user/main.opp : src/main.cpp	
${BIN_PLACE}user/Sensors.o : src/Sensors.c
${BIN_PLACE}user/COM_IO.opp : src/COM_IO.cpp

clean:
	@echo "CLEANING PROJECT"
	rm -f ${BIN_PLACE}${BINARY} ${OBJECTS} ${BIN_PLACE}${PROGRAM_NAME}.hex ${BIN_PLACE}${PROGRAM_NAME}.map
 
