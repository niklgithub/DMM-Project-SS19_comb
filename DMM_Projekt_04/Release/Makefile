################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../includes


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Demo.c \
../includes/dataflash.c \
../includes/fifo.c \
../includes/lcd.c \
../includes/music.c \
../includes/spi.c \
../includes/twi.c \
../includes/uart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Demo.o \
includes/dataflash.o \
includes/fifo.o \
includes/lcd.o \
includes/music.o \
includes/spi.o \
includes/twi.o \
includes/uart.o

OBJS_AS_ARGS +=  \
Demo.o \
includes/dataflash.o \
includes/fifo.o \
includes/lcd.o \
includes/music.o \
includes/spi.o \
includes/twi.o \
includes/uart.o

C_DEPS +=  \
Demo.d \
includes/dataflash.d \
includes/fifo.d \
includes/lcd.d \
includes/music.d \
includes/spi.d \
includes/twi.d \
includes/uart.d

C_DEPS_AS_ARGS +=  \
Demo.d \
includes/dataflash.d \
includes/fifo.d \
includes/lcd.d \
includes/music.d \
includes/spi.d \
includes/twi.d \
includes/uart.d

OUTPUT_FILE_PATH +=Demo.elf

OUTPUT_FILE_PATH_AS_ARGS +=Demo.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=16000000UL  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g3 -Wall -c -std=gnu99 -mmcu=atmega1284p -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

includes/%.o: ../includes/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=16000000UL  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g3 -Wall -c -std=gnu99 -mmcu=atmega1284p -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 5.4.0
	$(QUOTE)D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Demo.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atmega1284p  
	@echo Finished building target: $@
	"D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Demo.elf" "Demo.hex"
	"D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Demo.elf" "Demo.eep" || exit 0
	"D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Demo.elf" > "Demo.lss"
	"D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Demo.elf" "Demo.srec"
	"D:\Programme Studium\Programmieren\Atmel Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "Demo.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Demo.elf" "Demo.a" "Demo.hex" "Demo.lss" "Demo.eep" "Demo.map" "Demo.srec" "Demo.usersignatures"
	