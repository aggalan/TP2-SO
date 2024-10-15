include ../Makefile.inc

MODULE = 0000-sampleCodeModule.bin
SOURCES = $(wildcard [^_]*.c) $(wildcard */*.c)
SOURCES_ASM = $(wildcard asm/*.asm)
OBJECTS_C = $(SOURCES:.c=.o)
OBJECTS_ASM = $(SOURCES_ASM:asm/%.asm=asm/%.o)
OBJECTS = $(OBJECTS_C) $(OBJECTS_ASM)

all: $(MODULE)

$(MODULE): $(OBJECTS)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(OBJECTS) -o ../$(MODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

asm/%.o: asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o asm/*.o
	rm -rf ../$(MODULE)

.PHONY: all clean

