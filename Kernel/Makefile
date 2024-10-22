include Makefile.inc

KERNEL_ELF=kernel.elf
KERNEL_BIN=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

SOURCES_DRIVERS=$(wildcard Drivers/*.c)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)

SOURCES_BITMAP=$(wildcard memory_manager/*_bitmap.c)
OBJECTS_BITMAP=$(SOURCES_BITMAP:.c=.o)

SOURCES_BUDDY=$(wildcard memory_manager/*_buddy.c)
OBJECTS_BUDDY=$(SOURCES_BUDDY:.c=.o)

SOURCES_COLLECTIONS=$(wildcard collections/*.c)
OBJECTS_COLLECTIONS=$(SOURCES_COLLECTIONS:.c=.o)

LOADEROBJECT=$(LOADERSRC:.asm=.o)

ALL_OBJECTS=$(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP) $(OBJECTS_COLLECTIONS) $(OBJECTS_BUDDY)

# Define specific flags for kernel linking
KERNEL_LDFLAGS=-z max-page-size=0x1000 -T kernel.ld --oformat elf64-x86-64 -nostdlib

all: $(KERNEL_BIN)

$(KERNEL_ELF): $(ALL_OBJECTS)
	@echo "Linking ELF file..."
	$(LD) $(KERNEL_LDFLAGS) -o $@ $(ALL_OBJECTS)
	@echo "Verifying ELF file..."
	file $@ || true

$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "Creating binary..."
	objcopy -O binary -S $< $@
	@echo "Verifying binary file..."
	ls -l $@

%.o: %.c
	@echo "Compiling $<..."
	$(GCC) $(GCCFLAGS) -I./include -c $< -D$(MM) -o $@

%.o : %.asm
	@echo "Assembling $<..."
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	@echo "Building loader..."
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	@echo "Cleaning..."
	rm -rf asm/*.o *.o *.bin *.elf
	rm -rf Drivers/*.o
	rm -rf memory_manager/*.o
	rm -rf collections/*.o

.PHONY: all clean