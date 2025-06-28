TOOLCHAIN_PATH = F:\\xpack-aarch64-none-elf-gcc-14.2.1-1.1\\bin
ASM = aarch64-none-elf-gcc
CXX = aarch64-none-elf-gcc
LINKER = aarch64-none-elf-ld
OBJCPY = aarch64-none-elf-objcopy

CFILES = $(wildcard ./*.c)
OFILES = $(CFILES:.c=.o)
INCLUDES = -I./
GCCFLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles -mstrict-align
LDFLAGS = -nostdlib  $(INCLUDES)
DELETE = rm

all: clean kernel8.img

startup.o: startup.S
	$(TOOLCHAIN_PATH)/$(ASM) $(GCCFLAGS) -g -c startup.S -o startup.o

%.o: %.c
	$(TOOLCHAIN_PATH)/$(CXX) $(GCCFLAGS) -g -c $< -o $@

kernel8.img: startup.o $(OFILES)
	$(TOOLCHAIN_PATH)/$(LINKER) $(LDFLAGS) startup.o $(OFILES) -T link.ld -o kernel8.elf
	$(TOOLCHAIN_PATH)/$(OBJCPY) -O binary kernel8.elf kernel8.img

clean:
	$(DELETE) -rf kernel8.img
	$(DELETE) -rf startup.o kernel8.elf $(OFILES)