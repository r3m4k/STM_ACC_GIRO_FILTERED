# ---------------------------
# User files
# ---------------------------

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

build_user: ${USER_OBJ}
	@echo # ---------------------------
	@echo BUILD "${BIN_PLACE}/${BINARY}", MEMORY CARD "${BIN_PLACE}/${PROGRAM_NAME}.map"
	$(CC) ${LINK_FLAGS} -o ${BIN_PLACE}/$(BINARY) $(USER_OBJ) ${LIBS}
	${GCC_PLACE}/bin/arm-none-eabi-size --format=berkeley ${BIN_PLACE}/${BINARY}
	@echo # ---------------------------
	@echo FORMING "${BIN_PLACE}/${PROGRAM_NAME}.hex"
	${GCC_PLACE}/bin/arm-none-eabi-objcopy -O ihex ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex 

clean_user:
	@echo Deleting user's object files and generated files
	@rm -f ${USER_OBJ} ${BIN_PLACE}/${BINARY} ${BIN_PLACE}/${PROGRAM_NAME}.hex ${BIN_PLACE}/${PROGRAM_NAME}.map

rebuild_user: clean_user build_user

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

# --------------------------------