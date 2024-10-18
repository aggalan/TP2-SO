include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

SOURCES_DRIVERS=$(wildcard Drivers/*.c)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)

SOURCES_BITMAP= $(wildcard memory_manager/*_bitmap.c)
OBJECTS_BITMAP=$(SOURCES_BITMAP:.c=.o)

SOURCES_BUDDY=$(wildcard memory_manager/*_buddy.c)
OBJECTS_BUDDY=$(SOURCES_BUDDY:.c=.o)

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP) $(OBJECTS_ASM) $(STATICLIBS)

buddy: $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BUDDY)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_BUDDY) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin
	rm -rf drivers/*.o *.o *.bin
	rm -rf memory_manager/*.o
	rm -rf *.o *.bin

.PHONY: all clean buddy
