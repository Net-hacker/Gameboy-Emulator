#include <cpu.h>
#include <debug.h>
#include <memory.h>

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void Debugging(unsigned char* rom, CPU cpu)
{
  char input[5];
  char* converter = (char*) malloc(sizeof(char));

  do {
    printf("\n>> ");
    scanf("%4s", input);

    if (checkString(input, "A")) {
      converter[0] = (char) cpu.A;
      printDebug("Der Wert von Register A: ", "%d", converter[0]);
    } else if (checkString(input, "B")) {
      converter[0] = (char) cpu.B;
      printDebug("Der Wert von Register B: ", "%d", converter[0]);
    } else if (checkString(input, "C")) {
      converter[0] = (char) cpu.C;
      printDebug("Der Wert von Register C: ", "%d", converter[0]);
    } else if (checkString(input, "D")) {
      converter[0] = (char) cpu.D;
      printDebug("Der Wert von Register D: ", "%d", converter[0]);
    } else if (checkString(input, "E")) {
      converter[0] = (char) cpu.E;
      printDebug("Der Wert von Register E: ", "%d", converter[0]);
    } else if (checkString(input, "H")) {
      converter[0] = (char) cpu.H;
      printDebug("Der Wert von Register H: ", "%d", converter[0]);
    } else if (checkString(input, "L")) {
      converter[0] = (char) cpu.L;
      printDebug("Der Wert von Register L: ", "%d", converter[0]);
    } else if (checkString(input, "PC")) {
      converter[0] = (char) cpu.PC;
      printDebug("Der Wert von Register PC: ", "%d", converter[0]);
    } else if (checkString(input, "SP")) {
      converter[0] = (char) cpu.SP;
      printDebug("Der Wert von Register SP: ", "%d", converter[0]);
    } else if (checkString(input, "Mem")) {
      FILE *mem = fopen("Memory.txt", "w");
      fprintf(mem, "  ");
      for (size_t i = 0; i < sizeof(cpu.memory); i++) {
        fprintf(mem, "%02x ", cpu.memory[i]);
        if ((i + 1) % 16 == 0)
          fprintf(mem, "  %06X\n", i);
        if ((i + 1) % 8 == 0)
          fprintf(mem, "  ");
      }
      fclose(mem);
      printDebug("Memory Dumped", "");
    } else if (checkString(input, "Rom")) {
      FILE *roms = fopen("Rom.txt", "w");
      for (size_t i = 0; i < sizeof(rom); i++) {
        fprintf(roms, "%02X", rom[i]);
        if ((i + 1) % 16 == 0)
          fprintf(roms, "\n");
      }
      fclose(roms);
      printDebug("Rom Dumped", "");
    } else if (checkString(input, "help")) {
      printf("Register A-L: A B C ... L\n");
      printf("Pointers PC | SP: PC SP\n");
      printf("Memory Dump: Mem\n");
      printf("Rom Dump: Rom\n");
      printf("Help: help\n");
    }
  } while (!(checkString(input, "exit")));

  free(converter);
}

void Run(unsigned char* rom, CPU cpu, bool debug)
{
  uint8_t opcode = rom[cpu.PC];
  bool halted = false;

  while (!WindowShouldClose() && cpu.PC <= 50 && !halted) {
    printf("%02X\n", rom[cpu.PC]);
    switch (opcode) {
      case 0x00:
        printDebug("NOP", "");
        break;
      case 0x01: {
        cpu.BC = cpu.memory[cpu.PC + 1] << 8 | cpu.memory[cpu.PC + 2];
        cpu.B = (cpu.BC >> 8) & 0xFF;
        cpu.C = cpu.BC & 0xFF;
        cpu.PC += 2;
        printDebug("LD BC, u16", "");
        break;
      }
      case 0x02: {
        uint16_t address = (cpu.B << 8) | cpu.C;
        cpu.memory[address] = cpu.A;
        printDebug("LD (BC), A", "");
        break;
      }
      case 0x03: {
        cpu.BC = (cpu.B << 8) | cpu.C;
        cpu.BC;
        cpu.B = (cpu.BC >> 8) & 0xFF;
        cpu.C = cpu.BC & 0xFF;
        printDebug("INC BC", "");
        break;
      }
      case 0x04:
        uint8_t pre = cpu.B;
        cpu.B++;
        if (cpu.B == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC B", "");
        break;
      case 0x05: {
        uint8_t pre = cpu.B;
        cpu.B--;
        if (cpu.B == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC B", "");
        break;
      }
      case 0x06:
        cpu.B = cpu.memory[cpu.PC++];
        printDebug("LD B, u8", "");
        break;
      case 0x07: {
        uint8_t pre = cpu.A;
        cpu.A = (cpu.A << 1) | (pre >> 7);
        cpu.Flag &= ~(FLAG_Z | FLAG_N | FLAG_H);
        if (pre & 0x80)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        printDebug("RLCA", "");
        break;
      }
      case 0x08: {
        uint16_t address = cpu.memory[cpu.PC + 1] | cpu.memory[cpu.PC + 2];
        cpu.memory[address] = cpu.SP & 0xFF;
        cpu.memory[address++] = (cpu.SP >> 8) & 0xFF;
        cpu.PC += 2;
        printDebug("LD (u16), SP", "");
        break;
      }
      case 0x09: {
        cpu.BC = (cpu.B << 8) | cpu.C;
        cpu.HL = (cpu.H << 8) | cpu.L;
        uint32_t result = cpu.HL + cpu.BC;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;

        if (((cpu.HL & 0x0FFF) + (cpu.BC & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_N;
        else
          cpu.Flag &= ~FLAG_N;

        cpu.Flag &= ~FLAG_N;
        printDebug("ADD HL, BC", "");
        break;
      }
      case 0x0A: {
        uint16_t address = (cpu.B << 8) | cpu.C;
        cpu.A = cpu.memory[address];
        printDebug("LD A, (BC)", "");
        break;
      }
      case 0x0B: {
        cpu.BC = (cpu.B << 8) | cpu.C;
        cpu.BC--;
        cpu.B = (cpu.BC >> 8) & 0xFF;
        cpu.C = cpu.BC & 0xFF;
        printDebug("DEC BC", "");
        break;
      }
      case 0x0C: {
        uint8_t pre = cpu.C;
        cpu.C++;
        if (cpu.C == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC C", "");
        break;
      }
      case 0x0D: {
        uint8_t pre = cpu.C;
        cpu.C--;
        if (cpu.C == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC C", "");
        break;
      }
      case 0x0E:
        cpu.C = cpu.memory[cpu.PC++];
        printDebug("LD C, u8", "");
        break;
      case 0x0F: {
        uint8_t carry = (cpu.A >> 7) & 0x01;
        cpu.A = (cpu.A >> 1) | (carry << 7);
        cpu.Flag &= ~(FLAG_N | FLAG_Z | FLAG_H);
        if (carry)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        printDebug("RRCA", "");
        break;
      }
      case 0x10:
        //halted = true;
        printDebug("STOP", "");
        break;
      case 0x11: {
        cpu.DE = (cpu.memory[cpu.PC + 2] << 8) | cpu.memory[cpu.PC + 1];
        cpu.D = (cpu.DE >> 8) & 0xFF;
        cpu.E = cpu.DE & 0xFF;
        cpu.PC += 2;
        printDebug("LD DE, u16", "");
        break;
      }
      case 0x12: {
        uint16_t address = (cpu.D << 8) | cpu.E;
        cpu.memory[address] = cpu.A;
        printDebug("LD (DE), A", "");
        break;
      }
      case 0x13: {
        cpu.DE = (cpu.D << 8) | cpu.E;
        cpu.DE++;
        cpu.D = (cpu.DE >> 8) & 0xFF;
        cpu.E = cpu.DE & 0xFF;
        printDebug("INC DE", "");
        break;
      }
      case 0x14: {
        uint8_t pre = cpu.D;
        cpu.D++;
        if (cpu.D == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC D", "");
        break;
      }
      case 0x15: {
        uint8_t pre = cpu.D;
        cpu.D--;
        if (cpu.D == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC D", "");
        break;
      }
      case 0x16:
        cpu.D = cpu.memory[cpu.PC++];
        printDebug("LD D, u8", "");
        break;
      case 0x17: {
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint8_t new_carry = (cpu.A & 0x80) ? 1 : 0;
        cpu.A = (cpu.A << 1) | carry;
        cpu.Flag &= ~(FLAG_Z | FLAG_N | FLAG_H | FLAG_C);
        if (new_carry)
          cpu.Flag |= FLAG_C;
        printDebug("RLA", "");
        break;
      }
      case 0x18: {
        int8_t offset = cpu.memory[cpu.PC++];
        cpu.PC += offset;
        printDebug("JR i8", "");
        break;
      }
      case 0x19: {
        cpu.DE = (cpu.D << 8) | cpu.E;
        cpu.HL = (cpu.H << 8) | cpu.L;
        uint32_t result = cpu.HL + cpu.DE;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;

        if (((cpu.HL & 0x0FFF) + (cpu.DE & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_N;
        else
          cpu.Flag &= ~FLAG_N;

        cpu.Flag &= ~FLAG_N;
        printDebug("ADD HL, DE", "");
        break;
      }
      case 0x1A: {
        uint16_t address = (cpu.D << 8) | cpu.E;
        cpu.A = cpu.memory[address];
        printDebug("LD A, (DE)", "");
        break;
      }
      case 0x1B: {
        cpu.DE = (cpu.D << 8) | cpu.E;
        cpu.DE--;
        cpu.D = (cpu.DE >> 8) & 0xFF;
        cpu.E = cpu.DE & 0xFF;
        printDebug("DEC DE", "");
        break;
      }
      case 0x1C: {
        uint8_t pre = cpu.E;
        cpu.E++;
        if (cpu.E == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC E", "");
        break;
      }
      case 0x1D: {
        uint8_t pre = cpu.E;
        cpu.E--;
        if (cpu.E == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC E", "");
        break;
      }
      case 0x1E:
        cpu.E = cpu.memory[cpu.PC++];
        printDebug("LD E, u8", "");
        break;
      case 0x1F: {
        bool pre = (cpu.Flag & FLAG_C) ? true : false;
        uint8_t bit0 = cpu.A & 0x01;
        cpu.A = (cpu.A >> 1) | (pre << 7);
        cpu.Flag &= ~(FLAG_Z | FLAG_N | FLAG_H);
        if (bit0)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
      }
      case 0x20: {
        int8_t offset = cpu.memory[cpu.PC++];
        if (!(cpu.Flag & FLAG_Z))
          cpu.PC += offset;
        printDebug("JR NZ, i8", "");
        break;
      }
      case 0x21:
        cpu.HL = (cpu.memory[cpu.PC + 2] << 8) | cpu.memory[cpu.PC + 1];
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        cpu.PC += 2;
        printDebug("LD HL, u16", "");
        break;
      case 0x22:
        cpu.HL = (cpu.H << 8) | cpu.L;
        cpu.memory[cpu.HL] = cpu.A;
        cpu.HL++;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD (HL+), A", "");
        break;
      case 0x23:
        cpu.HL = (cpu.H << 8) | cpu.L;
        cpu.HL++;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("INC HL", "");
        break;
      case 0x24: {
        uint8_t pre = cpu.H;
        cpu.H++;
        cpu.Flag &= ~FLAG_N;
        if (cpu.H == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC H", "");
        break;
      }
      case 0x25: {
        uint8_t pre = cpu.H;
        cpu.H--;
        cpu.Flag |= FLAG_N;
        if (cpu.H == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC H", "");
        break;
      }
      case 0x26:
        cpu.H = cpu.memory[cpu.PC++];
        printDebug("LD H, u8", "");
        break;
      case 0x27: {
        uint8_t correction = 0;
        uint8_t a = cpu.A;
        if (cpu.Flag & FLAG_H || (a & 0x0F) > 9)
          correction |= 0x06;

        if (cpu.Flag & FLAG_C || a > 0x99)
          correction |= 0x60;

        if (cpu.Flag & FLAG_N)
          a -= correction;
        else
          a += correction;

        cpu.Flag &= ~(FLAG_H | FLAG_Z);
        if (a == 0)
          cpu.Flag |= FLAG_Z;

        if (correction & 0x60)
          cpu.Flag |= FLAG_C;

        cpu.A = a;
        printDebug("DAA", "");
        break;
      }
      case 0x28: {
        int8_t offset = cpu.memory[cpu.PC++];
        if (cpu.Flag & FLAG_Z)
          cpu.PC += offset;
        printDebug("JR Z, i8", "");
        break;
      }
      case 0x29: {
        cpu.HL = (cpu.H << 8) | cpu.L;
        uint32_t result = cpu.HL + cpu.HL;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        if (((cpu.HL & 0x0FFF) + (cpu.HL & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.Flag &= ~FLAG_N;
        printDebug("ADD HL, HL", "");
        break;
      }
      case 0x2A:
        cpu.HL = (cpu.H << 8) | cpu.L;
        cpu.A = cpu.memory[cpu.HL];
        cpu.HL++;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD A, (HL+)", "");
        break;
      case 0x2B:
        cpu.HL = (cpu.H << 8) | cpu.L;
        cpu.HL--;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("DEC HL", "");
        break;
      case 0x2C: {
        uint8_t pre = cpu.L;
        cpu.L++;
        cpu.Flag &= ~FLAG_N;
        if (cpu.L == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC L", "");
        break;
      }
      case 0x2D: {
        uint8_t pre = cpu.L;
        cpu.L--;
        cpu.Flag |= FLAG_N;
        if (cpu.L == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC L", "");
        break;
      }
      case 0x2E:
        cpu.L = cpu.memory[cpu.PC++];
        printDebug("LD L, u8", "");
        break;
      case 0x2F:
        cpu.A = ~cpu.A;
        cpu.Flag |= FLAG_N;
        cpu.Flag |= FLAG_H;
        printDebug("CPL", "");
        break;
      default:
        printError("OpCode not found!", "");
        break;
    }
    cpu.PC++;
    opcode = rom[cpu.PC];
  }

  if (debug) {
    Debugging(rom, cpu);
  }
}
