#include <cpu.h>
#include <debug.h>

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

void Run(unsigned char* rom, CPU cpu)
{
  uint8_t opcode = rom[cpu.PC];

  while (!WindowShouldClose() && cpu.PC <= 4) {
    printf("%02X\n", rom[cpu.PC]);
    switch (opcode) {
      case 0x00:
        printDebug("NOP");
        break;
      case 0x01:
        cpu.BC = rom[cpu.PC + 1];
        cpu.PC += 2;
        printDebug("LD BC, u16");
        break;
    }
    cpu.PC++;
    opcode = rom[cpu.PC];
  }
}
