#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>

typedef struct {
  uint8_t A, B, C, D, E, H, L;
  uint16_t BC, DE, HL, PC, SP;
} CPU;

extern uint8_t opcode;

void Run(unsigned char* rom, CPU cpu);

#endif
