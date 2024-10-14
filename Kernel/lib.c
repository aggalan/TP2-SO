#include <stdint.h>
#include <time.h>
#include "include/interrupts.h"
#include "include/processManager.h"

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

void sleepms(uint32_t ms){
    uint32_t start = ticks_elapsed();
    while (ms > 18* (ticks_elapsed() - start)){
		_hlt();
	}
}
int str_len(char * str){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
};

int str_cpy(char * dest, const char * src){
    int i = 0;
    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    return i;
}

void idle(){
	while(1){
		_hlt();
	}
}

void exit_process() {
    PCB * pcb = get_current();
//    pcb->process->state = KILLED;
    kill_process_pid(pcb->process->pid);
//    _irq00Handler();
}