// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdint.h>
#include <lib.h>
#include <module_loader.h>

static void load_module(uint8_t **module, void *target_module_address);
static uint32_t readUint32(uint8_t **address);

void load_modules(void *payload_start, void **target_module_address)
{
	int i;
	uint8_t *current_module = (uint8_t *)payload_start;
	uint32_t module_count = readUint32(&current_module);

	for (i = 0; i < module_count; i++)
		load_module(&current_module, target_module_address[i]);
}

static void load_module(uint8_t **module, void *target_module_address)
{
	uint32_t module_size = readUint32(module);

	memcpy(target_module_address, *module, module_size);
	*module += module_size;
}

static uint32_t readUint32(uint8_t **address)
{
	uint32_t result = *(uint32_t *)(*address);
	*address += sizeof(uint32_t);
	return result;
}
