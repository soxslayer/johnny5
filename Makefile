SRCS := \
  assert.c \
  atomic.c \
  basic_uart.c \
  bits.c \
  buff.c \
  chardev.c \
  clock.c \
  ctx_switch.s \
  div0.c \
  event.c \
  fault.s \
  handle_array.c \
  hash_table.c \
  heap.c \
  idle.c \
  init.c \
  intrinsic.s \
  list.c \
  longjmp.c \
  mem.c \
  nvic.c \
  pdc.c \
  peripheral.c \
  pio.c \
  sem.c \
  signal.c \
  spinlock.c \
  string.c \
  syscall.c \
  task.c \
  uart.c \
  uidivmod.s \
  uldivmod.s \
  vec_tbl.c \
  volcfg.c \
  wdt.c

vpath %.h kernel
vpath %.c kernel
vpath %.s kernel

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
  -mcpu=cortex-m3 \
  -mthumb \
  -mno-thumb-interwork \
  -mfix-cortex-m3-ldrd \
  -fno-omit-frame-pointer \
  -mapcs-frame \
  -g
LDFLAGS:= \
  -T $(MEMORY_LAYOUT)
ASFLAGS:= \
  --warn \
  --fatal-warnings \
  -mcpu=cortex-m3 \
  -mthumb \
  -g

ifdef TEST
SRCS += test_main.c
CPPFLAGS += -DTEST=1
endif

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
