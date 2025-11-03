#ifndef _CPU_H
#define _CPU_H

#define MEM_SIZE 0x10000

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t A, B, C, D, E, H, L;
  uint8_t F;
  uint16_t BC, DE, HL, PC, SP;
  uint8_t memory[MEM_SIZE];
} CPU;

extern uint8_t opcode;

void Debugging(unsigned char* rom, CPU cpu);
void Run(unsigned char* rom, CPU cpu, bool debug);

#endif
