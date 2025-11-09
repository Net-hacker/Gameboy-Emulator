#ifndef _CPU_H
#define _CPU_H

#define MEM_SIZE 0x10000  //65 KB

#define FLAG_C 0x10
#define FLAG_H 0x20
#define FLAG_N 0x40
#define FLAG_Z 0x80

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t A, B, C, D, E, H, L, Flag;
  uint16_t BC, DE, HL, PC, SP;
  uint8_t memory[MEM_SIZE];
} CPU;

extern uint8_t opcode;

void Debugging(unsigned char* rom, CPU cpu, long size);
void Run(unsigned char* rom, CPU cpu, bool debug, long size);

#endif
