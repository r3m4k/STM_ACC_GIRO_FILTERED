# ---------------------------
# STM_ACC_GIRO_FILTERED
# ---------------------------

include make_args.mk
include make_StmLibs.mk
include make_UsbLib.mk

# Включаемые директории в файлах пользователя
USER_INCLUDES = \
-I"${GCC_PLACE}/arm-none-eabi/include" \
-I"include" \
-I"src" \
-I"system/include" \
-I"system/include/cmsis" \
-I"system/include/stm32f3-stdperiph" \
-I"system/include/additionally" \
-I"system/USB_LIB/include"

USER_DEFINES =

# ----------------------------
# Исходные файлы пользователя
# ----------------------------

USER_DIR = src

USER_SRC = \
${USER_DIR}/Drv_Gpio.cpp \
${USER_DIR}/Drv_Uart.cpp \
${USER_DIR}/main.cpp \
${USER_DIR}/Sensors.c \
${USER_DIR}/COM_IO.cpp

# ----------------------------
# Объектные файлы пользователя
# ----------------------------

USER_OBJ_DIR = ${BIN_PLACE}/user
USER_OBJ = \
$(patsubst ${USER_DIR}/%.c, ${USER_OBJ_DIR}/%.o,${USER_SRC})

# ---------------------------
# Targets
# ---------------------------
${BIN_PLACE}/$(BINARY) : $(OBJECTS)
	echo "BUILD "${BIN_PLACE}/${BINARY}", MEMORY CARD "${BIN_PLACE}/${PROGRAM_NAME}.map
	$(CC) ${LINK_FLAGS} -o ${BIN_PLACE}/$(BINARY) $(OBJECTS) ${LIBS}
	${GCC_PLACE}/bin/arm-none-eabi-size --format=berkeley ${BIN_PLACE}/${BINARY}
	@echo "FORMING "${BIN_PLACE}/${PROGRAM_NAME}.hex 
	${GCC_PLACE}/bin/arm-none-eabi-objcopy -O ihex ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex 

# %.o:
# 	echo "gcc "$<
# 	$(CC) ${GCC_FLAGS} ${DEFINES} ${INCLUDES}  $< -o $@

# %.opp:
# 	echo "g++ "$<
# 	$(CP) ${GPP_FLAGS} ${DEFINES} ${INCLUDES} $< -o $@


# ${BIN_PLACE}/$(BINARY) : $(USER_OBJ) STM_LIBS
# 	echo "BUILD "${BIN_PLACE}/${BINARY}", MEMORY CARD "${BIN_PLACE}/${PROGRAM_NAME}.map
# 	$(CC) ${LINK_FLAGS} -o ${BIN_PLACE}/$(BINARY) $(USER_OBJ) ${LIBS} -lm
# 	${GCC_PLACE}/bin/arm-none-eabi-size --format=berkeley ${BIN_PLACE}/${BINARY}
# 	@echo "FORMING "${BIN_PLACE}/${PROGRAM_NAME}.hex 
# 	${GCC_PLACE}/bin/arm-none-eabi-objcopy -O ihex ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex 


clean:
	@echo "CLEANING PROJECT"
	rm -f ${BIN_PLACE}/${BINARY} ${OBJECTS} ${BIN_PLACE}/${PROGRAM_NAME}.hex ${BIN_PLACE}/${PROGRAM_NAME}.map

# --------------------------------
# Создание необходимых директорий
# --------------------------------

# Функция для создания директории с проверкой её наличие перед этим
ensure_dir =
ifeq ($(SYSTEM), windows)
	ensure_dir = $(if $(wildcard $(1)/.),$(info Directory "$(1)" already exists),$(shell mkdir $(subst /,\,$(1))))
else ifeq ($(SYSTEM), linux)
	ensure_dir = $(if $(wildcard $(1)/.),$(info Directory "$(1)" already exists),$(shell mkdir $(1)))
endif

create_subdirs: create_bin_dir
	$(foreach dir, ${SUBDIRS_OBJ}, $(call ensure_dir,$(dir)))

create_bin_dir:
	$(call ensure_dir,Debug_Win)

# --------------------------------

STM_LIBS: STM32_STD_LIB STM32_USB_LIB
	@echo The ${STM_STD_LIB_NAME} is built
