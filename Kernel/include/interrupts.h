/*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idt_loader.h>

#include <stdint.h>

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _irq80Handler(void);

void _exception0Handler(void);
void _exception6Handler(void);
void _cli(void);
void save_registers_state();
uint64_t *get_registers();
short get_flag();
void print_registers_asm(uint32_t colour);

void _sti(void);

void _hlt(void);

void pic_master_mask(uint8_t mask);

void pic_slave_mask(uint8_t mask);

// Termina la ejecución de la cpu.
void haltcpu(void);

void nice();

#endif /* INTERRUPS_H_ */
