# ---------------------------
# STM_ACC_GIRO_FILTERED
# ---------------------------

include make_args.mk
include make_StmLib.mk
include make_UsbLib.mk

# ----------------------------
SUBDIRS_OBJ += ${USER_OBJ_DIR}

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

USER_SRC_C = \
${USER_DIR}/Sensors.c

USER_SRC_CPP = \
${USER_DIR}/Drv_Gpio.cpp \
${USER_DIR}/Drv_Uart.cpp \
${USER_DIR}/main.cpp \
${USER_DIR}/COM_IO.cpp
 
# ----------------------------
# Объектные файлы пользователя
# ----------------------------

USER_OBJ_DIR = ${BIN_PLACE}/user
USER_OBJ = \
$(patsubst ${USER_DIR}/%.c, ${USER_OBJ_DIR}/%.o,${USER_SRC_C}) \
$(patsubst ${USER_DIR}/%.cpp, ${USER_OBJ_DIR}/%.opp,${USER_SRC_CPP}) \

# ----------------------------

${USER_OBJ_DIR}/%.o: ${USER_DIR}/%.c
	@echo Compiling $@ from $<
	@${CC} ${GCC_FLAGS} ${DEFINES} ${USER_DEFINES} ${INCLUDES_USB_LIB} $< -o $@

${USER_OBJ_DIR}/%.opp: ${USER_DIR}/%.cpp
	@echo Compiling $@ from $<
	@${CP} ${GPP_FLAGS} ${DEFINES} ${USER_DEFINES} ${INCLUDES_USB_LIB} $< -o $@

# ---------------------------
# Правила
# ---------------------------

build_all: create_dirs STM_LIBS build_user
	@echo # ---------------------------
	@echo The project building is completed
	@echo # ---------------------------

clean_all: clean_user clean_libs
	@echo The project has been cleared

rebuild_all: clean_all build_all

# ---------------------------

build_user: ${USER_OBJ}
	@echo "BUILD "${BIN_PLACE}/${BINARY}", MEMORY CARD "${BIN_PLACE}/${PROGRAM_NAME}.map
	$(CC) ${LINK_FLAGS} -o ${BIN_PLACE}/$(BINARY) $(USER_OBJ) ${LIBS}
	${GCC_PLACE}/bin/arm-none-eabi-size --format=berkeley ${BIN_PLACE}/${BINARY}
	@echo "FORMING "${BIN_PLACE}/${PROGRAM_NAME}.hex 
	${GCC_PLACE}/bin/arm-none-eabi-objcopy -O ihex ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex 

clean_user:
	rm -f ${USER_OBJ} ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex ${BIN_PLACE}/${PROGRAM_NAME}.map

# ---------------------------

STM_LIBS: STM32_STD_LIB STM32_USB_LIB
	@echo # ---------------------------
	@echo The building of libraries is completed
	@echo # ---------------------------

clean_libs: clean_stm_std_lib clean_usb_lib

# --------------------------------
# Создание необходимых директорий
# --------------------------------

create_dirs: create_bin_dir create_subdirs
	@echo All directories are created

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

info_user:
	@echo # ---------------------------
	@echo # Building user files and linking project
	@echo # ---------------------------
	@echo Defines: ${DEFINES}
	@echo User_DEFINES: ${USER_DEFINES}
	@echo User_Includes: ${USER_INCLUDES}
	@echo GCC_FLAGS: ${GCC_FLAGS}
	@echo GPP_FLAGS: ${GPP_FLAGS}
	@echo # ---------------------------
