#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <module_loader.h>
#include <naive_console.h>
#include <syscalls.h>
#include "../Drivers/include/video_driver.h"
#include "idt_loader.h"
#include "./memory_manager/include/memory_manager.h"
#include "process_manager.h"
#include "scheduler.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void *retUserLand()
{
	return sampleCodeModuleAddress;
}

void *initializeKernelBinary()
{
	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

void shell()
{
	char *argv[] = {"shell"};
	create_process((uint64_t)sampleCodeModuleAddress, 5, 1, argv);
}

int main()
{
	load_idt();
	mm_init((void *)0x600000, 0x2700000);
	scheduler_init();
	shell();

	while (1)
		;
	return 0;
}
