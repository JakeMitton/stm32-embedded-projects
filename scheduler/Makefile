TARGET = scheduler

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
FLASH_ADDR = 0x08000000

CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -O0 -g -MMD -MP
LDFLAGS = -T linker.ld -nostdlib -nostartfiles

SRCS_C = startup.c main.c globals.c systick.c scheduler.c
SRCS_S = 
OBJS = $(SRCS_C:.c=.o) $(SRCS_S:.S=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET).bin $(TARGET).elf

flash: $(TARGET).bin
	st-flash --reset write $< $(FLASH_ADDR)

size: $(TARGET).elf
	$(SIZE) -A $<

disassemble: $(TARGET).elf
	$(OBJDUMP) -d $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.d *.elf *.bin

.PHONY: all clean flash size disassemble

-include $(DEPS)
