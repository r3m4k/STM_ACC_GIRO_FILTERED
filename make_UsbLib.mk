# ---------------------------
# STM_USB_LIB 
# ---------------------------

STM_USB_LIB_NAME = libstm_usb_lib.a
LIBS += -L./${BIN_PLACE} -lstm_usb_lib

SUBDIRS_OBJ += ${USB_OBJ_DIR}

# ---------------------------

INCLUDES_USB_LIB = \
-I "system/USB_LIB/include" \
-I "system/include" \
-I "system/include/stm32f3-stdperiph" \
-I "system/include/additionally" \
-I "system/include/cmsis" \
-I "include" \
-I "src"

# ---------------------------
# Исходные файлы
# ---------------------------

USB_SRC_DIR = system/USB_LIB/src

USB_SRC_C = \
${USB_SRC_DIR}/hw_config.c \
${USB_SRC_DIR}/usb_istr.c \
${USB_SRC_DIR}/usb_pwr.c \
${USB_SRC_DIR}/usb_init.c \
${USB_SRC_DIR}/usb_prop.c \
${USB_SRC_DIR}/usb_regs.c \
${USB_SRC_DIR}/usb_desc.c \
${USB_SRC_DIR}/usb_sil.c \
${USB_SRC_DIR}/usb_core.c \
${USB_SRC_DIR}/usb_int.c \
${USB_SRC_DIR}/usb_mem.c \
${USB_SRC_DIR}/usb_endp.c

USB_SRC_CPP = \
${USB_SRC_DIR}/VCP_F3.cpp

# ---------------------------
# Объектные файлы
# ---------------------------
USB_OBJ_DIR = ${BIN_PLACE}/usb

USB_OBJ := \
$(patsubst ${USB_SRC_DIR}/%.c, ${USB_OBJ_DIR}/%.o,${USB_SRC_C}) \
$(patsubst ${USB_SRC_DIR}/%.cpp,${USB_OBJ_DIR}/%.opp,${USB_SRC_CPP})

# ---------------------------
# Правила
# ---------------------------
rebuild_stm_usb_lib: clean_usb_lib info_usb_lib STM32_USB_LIB
	@echo # ---------------------------
	@echo Rebuilding ${STM_USB_LIB_NAME} completed successfully
	@echo # ---------------------------

# ---------------------------
STM32_USB_LIB: ${USB_OBJ}
	@echo Building static library ${STM_USB_LIB_NAME}
	ar rcs ${BIN_PLACE}/${STM_USB_LIB_NAME} ${USB_OBJ}
	
# ---------------------------
${USB_OBJ_DIR}/%.o: ${USB_SRC_DIR}/%.c
	@echo Compiling $@ from $<
	@${CC} ${GCC_FLAGS} ${DEFINES} ${INCLUDES_USB_LIB} $< -o $@

${USB_OBJ_DIR}/%.opp: ${USB_SRC_DIR}/%.cpp
	@echo Compiling $@ from $<
	@${CP} ${GPP_FLAGS} ${DEFINES} ${INCLUDES_USB_LIB} $< -o $@

# ---------------------------
info_usb_lib:
	@echo # ---------------------------
	@echo # Building static library libstm_usb_lib.a
	@echo # ---------------------------
	@echo DEFINES: ${DEFINES}
	@echo Includes: ${INCLUDES_USB_LIB}
	@echo GCC_FLAGS: ${GCC_FLAGS}
	@echo GPP_FLAGS: ${GPP_FLAGS}
	@echo # ---------------------------

# ---------------------------
clean_usb_lib:
	rm -f ${USB_OBJ} ${BIN_PLACE}/${STM_USB_LIB_NAME}

# ---------------------------