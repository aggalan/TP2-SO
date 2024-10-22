include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin
SAMPLE_CODE=0000-sampleCodeModule

all: sampleCodeModule sampleDataModule

sampleCodeModule:
	cd SampleCodeModule; make
	# Create ELF file from the binary
	$(LD) -r -b binary -o $(SAMPLE_CODE).elf $(SAMPLE_CODE).bin

sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin *.elf


.PHONY: sampleCodeModule all clean