// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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
#include "semaphores.h"
#include "include/pipe_manager.h"
#include "include/fd_manager.h"

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_binary;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;

static void *const sample_code_module_address = (void *)0x400000;
static void *const sample_data_module_address = (void *)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *get_stack_base()
{
	return (void *)((uint64_t)&end_of_kernel + page_size * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)						 // Begin at the top of the stack
	);
}

void *ret_userland()
{
	return sample_code_module_address;
}

void *initialize_kernel_binary()
{
	void *moduleAddresses[] = {
		sample_code_module_address,
		sample_data_module_address};

	load_modules(&end_of_kernel_binary, moduleAddresses);

	clearBSS(&bss, &end_of_kernel - &bss);

	return get_stack_base();
}

void shell()
{
	char *argv[] = {"shell"};
	create_process((uint64_t)sample_code_module_address, 0, 1, argv, 0);
}

int main()
{
	load_idt();
	mm_init((void *)0x600000, 0x2700000);
	sem_manager();
	pipe_table_init();
	fd_init();
	scheduler_init();
	shell();

	while (1)
		;
	return 0;
}
