
BUILD_DIR:=build
DEVICE := atmega32u4
CPU_FREQ := 16000000


MODULES:= src 
INCLUDES:= include 
EXCLUDES:= 

INCLUDES += . $(MODULES)

SRC:= $(wildcard *.c *.cc *.s)
SRC += $(foreach sdir,$(MODULES),$(wildcard $(sdir)/*.c $(sdir)/*.cc $(sdir)/*.s))
SRC:= $(filter-out $(EXCLUDES),$(SRC))

OBJ:= $(patsubst %.c,%.o,$(SRC))
OBJ:= $(patsubst %.cc,%.o,$(OBJ))
OBJ:= $(patsubst %.s,%.o,$(OBJ))
OBJ:= $(patsubst %.o, build/%.o, $(notdir $(OBJ)))
VPATH:=$(MODULES)

CFLAGS :=  -Wall -Os -DCPU_FREQ=$(CPU_FREQ) -mmcu=$(DEVICE) 
CFLAGS += $(addprefix -I ,$(INCLUDES))
LDFLAGS :=     -D$(DEVICE) -mmcu=$(DEVICE)

DEPS = $(OBJ:.o=.d)

CC := avr-gcc 
CXX := avr-g++
LD := avr-ld
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump

FLASH := dfu-programmer $(DEVICE)



all: checkdirs hex bin

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: $(BUILD_DIR)/main.hex 

bin: $(BUILD_DIR)/main.bin


# rule for uploading firmware:
flash: $(BUILD_DIR)/main.hex
	$(FLASH) flash $(BUILD_DIR)/main.hex

run:
	$(FLASH) start

# Generic rule for compiling C files:
$(BUILD_DIR)/%.o: %.c
	$(CC) -std=c99  $(CFLAGS)  -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 

$(BUILD_DIR)/%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 
    
# Generic rule for assembling Assembler source files:
$(BUILD_DIR)/%.o: %.s
	$(CC) $(CFLAGS)  -c $< -o $@
	$(CC) $(CFLAGS) -MM $< > $(BUILD_DIR)/$*.d 


# file targets:


$(BUILD_DIR)/main.elf:  $(OBJ)	
	$(CC)  $(LDFLAGS)  -o $(BUILD_DIR)/main.elf $(OBJ)

$(BUILD_DIR)/main.hex: $(BUILD_DIR)/main.elf
	rm -f $(BUILD_DIR)/main.hex $(BUILD_DIR)/main.eep.hex
	$(OBJCOPY) -Oihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex

$(BUILD_DIR)/main.bin: $(BUILD_DIR)/main.elf
	rm -f $(BUILD_DIR)/main.bin
	$(OBJCOPY)  -Obinary $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.bin

# debugging targets:

disasm:	$(BUILD_DIR)/main.elf
	$(OBJDUMP) -S $(BUILD_DIR)/main.elf


checkdirs:$(BUILD_DIR)


$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)


-include	$(DEPS)
