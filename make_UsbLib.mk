# ---------------------------
# STM_USB_LIB 
# ---------------------------

INCLUDES_USB_LIB = -I "system/USB_LIB/include"

# Директория исходных файлов
USB_SRC_DIR = system/USB_LIB/src

# Исходные файлы
USB_SRC = \
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
${USB_SRC_DIR}/usb_endp.c \
${USB_SRC_DIR}/VCP_F3.cpp

# Директория объектных файлов
USB_OBJ_DIR = ${BIN_PLACE}/usb

# Объектные файлы
USB_OBJ = \
$(patsubst ${USB_SRC_DIR}/%.c, ${USB_OBJ_DIR}/%.o,${USB_SRC}) \
$(patsubst ${USB_SRC_DIR}/%.cpp,${USB_OBJ_DIR}/%.opp,${USB_SRC})

STM32_USB_LIB:
	@echo "STM32_USB_LIB"
	@echo "USB_OBJ: ${USB_OBJ}"
	
# # ---------------------------
# ${OBJ_USB_LIB}/%.o: ${USB_SRC}/%.c:
# 	@echo Compiling USB c-files
# 	${CC} ${GCC_FLAGS} ${DEFINES} ${INCLUDES_USB_LIB} $< -o $@

# ${OBJ_USB_LIB}/%.opp: ${USB_SRC}/%.cpp:
# 	@echo Compiling USB c-files
# 	${CC} ${GPP_FLAGS} ${DEFINES} ${INCLUDES_USB_LIB} $< -o $@
# # ---------------------------
