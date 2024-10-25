include Makefile.inc

KERNEL=kernel.bin
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

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP) $(OBJECTS_COLLECTIONS) $(OBJECTS_BUDDY)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BITMAP) $(OBJECTS_BUDDY) $(OBJECTS_COLLECTIONS)

buddy: $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BUDDY) $(OBJECTS_COLLECTIONS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_BUDDY) $(OBJECTS_COLLECTIONS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -D$(MM) -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin
	rm -rf Drivers/*.o
	rm -rf memory_manager/*.o
	rm -rf collections/*.o
	rm -rf *.o *.bin

.PHONY: all clean buddy