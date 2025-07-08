TOOLCHAIN_PATH = F:\\xpack-aarch64-none-elf-gcc-14.2.1-1.1\\bin
ASM = aarch64-none-elf-gcc
CXX = aarch64-none-elf-gcc
LINKER = aarch64-none-elf-ld
OBJCPY = aarch64-none-elf-objcopy
BUILD_DIR = build
SRCS = $(wildcard *.c)
SFILES = $(wildcard asm/*.S)
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS)) $(patsubst asm/%.S, $(BUILD_DIR)/%.o, $(SFILES))
INCLUDES = -I./
GCCFLAGS =  -Wall -O0 -ffreestanding -nostdlib -nostartfiles -mstrict-align
LDFLAGS = -nostdlib  $(INCLUDES)
DELETE = rm

all: clean createobj kernel8.img

$(BUILD_DIR)/%.o: asm/%.S
	$(TOOLCHAIN_PATH)/$(ASM) $(GCCFLAGS) -g -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(TOOLCHAIN_PATH)/$(CXX) $(GCCFLAGS) -g -c $^ -o $@
kernel8.img: $(OBJS)
	$(TOOLCHAIN_PATH)/$(LINKER) $(LDFLAGS) $(OBJS) -T link.ld -o kernel8.elf
	$(TOOLCHAIN_PATH)/$(OBJCPY) -O binary kernel8.elf kernel8.img

clean:
	$(DELETE) -rf kernel8.img
	$(DELETE) -rf kernel8.elf $(OBJS)
	$(DELETE) -rf $(BUILD_DIR)

createobj:
	mkdir $(BUILD_DIR)