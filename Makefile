SRCS:= \
  clock.c \
  init.c \
  int_vector.c \
  main.c \
  vector_tbl.c

vpath %.h src
vpath %.c src
vpath %.s src

ARMGNU:=arm-none-eabi

CC:=$(ARMGNU)-gcc
CPP:=$(ARMGNU)-gcc
AS:=$(ARMGNU)-as
LD:=$(ARMGNU)-ld
SIZE:=$(ARMGNU)-size
OBJCOPY:=$(ARMGNU)-objcopy

TARGET:=main
TARGET_ELF:=$(TARGET).elf
TARGET_MAP:=$(TARGET).map
TARGET_BIN:=$(TARGET).bin

MEMORY_LAYOUT:=sam3x8e.ld

CPPFLAGS:=
CFLAGS:= \
  -Wall \
  -Werror \
  -Wno-unused-function \
  -nostdlib \
  -nostartfiles \
  -ffreestanding \
  -mhard-float \
  -mcpu=cortex-m3 \
  -mthumb \
  -mno-thumb-interwork \
  -mfix-cortex-m3-ldrd \
  -g
LDFLAGS:= \
  -T $(MEMORY_LAYOUT)
ASFLAGS:= \
  --warn \
  --fatal-warnings \
  -mfloat-abi=hard \
  -mcpu=cortex-m3 \
  -mthumb \
  -g

OBJS:=$(SRCS:%.c=%.o)
OBJS:=$(OBJS:%.s=%.o)
DEP_FILES:=$(OBJS:%.o=%.d)

.PHONY: all
all: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) $^ -R .ARM.attributes -R .comment -O binary $@

$(TARGET_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET_ELF) $(OBJS)
	$(SIZE) $(TARGET_ELF)

%.o: %.c %.d
	$(CC) $(CPPFLAGS) -M -MG $< | sed 's|\($*.o\):|\1 $@:|g' > $*.d
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

%.d: ;

-include $(DEP_FILES)

.PHONY: clean
clean:
	rm -rf *.o *.d $(TARGET_ELF) $(TARGET_BIN) $(TARGET_MAP)