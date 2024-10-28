#include "include/sound_driver.h"
#include "../include/lib.h"

extern uint8_t inb(uint16_t in1);
extern void outb(uint16_t out1, uint8_t in1);
// Play sound using built-in speaker
static void play_sound(uint32_t nFrequence)
{
	uint32_t Div;
	uint8_t tmp;

	// Set the PIT to the desired frequency
	Div = 1193180 / nFrequence;
	outb(0x43, 0xb6);
	outb(0x42, (uint8_t)(Div));
	outb(0x42, (uint8_t)(Div >> 8));

	// And play the sound using the PC speaker
	tmp = inb(0x61);
	if (tmp != (tmp | 3))
	{
		outb(0x61, tmp | 3);
	}
}

// make it shut up
static void nosound()
{
	uint8_t tmp = inb(0x61) & 0xFC;

	outb(0x61, tmp);
}

// Make a beep
void beep()
{
	play_sound(100);
	sleepms(10);
	nosound();
}

// se utilizoz el codigo de https://wiki.osdev.org/PC_Speaker