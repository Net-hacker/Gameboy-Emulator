#include <cpu.h>
#include <debug.h>

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Der Debugger nach der Ausführung.
 *
 * Zum Debuggen des Programs da. Register können gedumpt werden genauso wie Speicher, PC, SP und die .gb Datei.
 *
 * @param rom Der Inhalt der .gb Datei als unsigned char*.
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 */
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

/**
 * Der Interpreter von OpCodes.
 *
 * Hier werden die eingelesen OpCodes interpretiert und ausgeführt.
 *
 * @param rom Der Inhalt der .gb Datei als unsigned char*.
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 * @param debug Wenn true, springt am Ende des Programms zum DebugMenu.
 */
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
        uint16_t address = cpu.BC;
        cpu.memory[address] = cpu.A;
        printDebug("LD (BC), A", "");
        break;
      }
      case 0x03: {
        cpu.BC++;
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
        cpu.BC = (cpu.B << 8) | cpu.C;
        printDebug("INC B", "");
        break;
      case 0x05: {
        uint8_t pre = cpu.B;
        cpu.B--;
        cpu.Flag |= FLAG_N;
        if (cpu.B == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        cpu.BC = (cpu.B << 8) | cpu.C;
        printDebug("DEC B", "");
        break;
      }
      case 0x06:
        cpu.B = cpu.memory[cpu.PC++];
        cpu.BC = (cpu.B << 8) | cpu.C;
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
        cpu.memory[address + 1] = (cpu.SP >> 8) & 0xFF;
        cpu.PC += 2;
        printDebug("LD (u16), SP", "");
        break;
      }
      case 0x09: {
        uint32_t result = cpu.HL + cpu.BC;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;

        if (((cpu.HL & 0x0FFF) + (cpu.BC & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        cpu.Flag &= ~FLAG_N;
        printDebug("ADD HL, BC", "");
        break;
      }
      case 0x0A: {
        uint16_t address = cpu.BC;
        cpu.A = cpu.memory[address];
        printDebug("LD A, (BC)", "");
        break;
      }
      case 0x0B: {
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
        cpu.BC = (cpu.B << 8) | cpu.C;
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
        cpu.BC = (cpu.B << 8) | cpu.C;
        printDebug("DEC C", "");
        break;
      }
      case 0x0E:
        cpu.C = cpu.memory[cpu.PC++];
        cpu.BC = (cpu.B << 8) | cpu.C;
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
        uint16_t address = cpu.DE;
        cpu.memory[address] = cpu.A;
        printDebug("LD (DE), A", "");
        break;
      }
      case 0x13: {
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
        cpu.DE = (cpu.D << 8) | cpu.E;
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
        cpu.DE = (cpu.D << 8) | cpu.E;
        printDebug("DEC D", "");
        break;
      }
      case 0x16:
        cpu.D = cpu.memory[cpu.PC++];
        cpu.DE = (cpu.D << 8) | cpu.E;
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
        uint32_t result = cpu.HL + cpu.DE;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;

        if (((cpu.HL & 0x0FFF) + (cpu.DE & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        cpu.Flag &= ~FLAG_N;
        printDebug("ADD HL, DE", "");
        break;
      }
      case 0x1A: {
        uint16_t address = cpu.DE;
        cpu.A = cpu.memory[address];
        printDebug("LD A, (DE)", "");
        break;
      }
      case 0x1B: {
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
        cpu.DE = (cpu.D << 8) | cpu.E;
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
        cpu.DE = (cpu.D << 8) | cpu.E;
        printDebug("DEC E", "");
        break;
      }
      case 0x1E:
        cpu.E = cpu.memory[cpu.PC++];
        cpu.DE = (cpu.D << 8) | cpu.E;
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
        printDebug("RRA", "");
        break;
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
        cpu.memory[cpu.HL] = cpu.A;
        cpu.HL++;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD (HL+), A", "");
        break;
      case 0x23:
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
        cpu.HL = (cpu.H << 8) | cpu.L;
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
        cpu.HL = (cpu.H << 8) | cpu.L;
        printDebug("DEC H", "");
        break;
      }
      case 0x26:
        cpu.H = cpu.memory[cpu.PC++];
        cpu.HL = (cpu.H << 8) | cpu.L;
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
        cpu.A = cpu.memory[cpu.HL];
        cpu.HL++;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD A, (HL+)", "");
        break;
      case 0x2B:
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
        cpu.HL = (cpu.H << 8) | cpu.L;
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
        cpu.HL = (cpu.H << 8) | cpu.L;
        printDebug("DEC L", "");
        break;
      }
      case 0x2E:
        cpu.L = cpu.memory[cpu.PC++];
        cpu.HL = (cpu.H << 8) | cpu.L;
        printDebug("LD L, u8", "");
        break;
      case 0x2F:
        cpu.A = ~cpu.A;
        cpu.Flag |= FLAG_N;
        cpu.Flag |= FLAG_H;
        printDebug("CPL", "");
        break;
      case 0x30: {
        int8_t offset = cpu.memory[cpu.PC++];
        if (!(cpu.Flag & FLAG_C))
          cpu.PC += offset;
        printDebug("JR NC, i8", "");
        break;
      }
      case 0x31:
        cpu.SP = (cpu.memory[cpu.PC + 2] << 8) | cpu.memory[cpu.PC + 1];
        cpu.PC += 2;
        printDebug("LD SP, u16", "");
        break;
      case 0x32:
        cpu.memory[cpu.HL] = cpu.A;
        cpu.HL--;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD (HL-), A", "");
        break;
      case 0x33:
        cpu.SP++;
        printDebug("INC SP", "");
        break;
      case 0x34: {
        uint8_t value = cpu.memory[cpu.HL];
        uint8_t pre = value;
        value++;
        cpu.Flag &= ~FLAG_N;
        if (value == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        cpu.memory[cpu.HL] = value;
        printDebug("INC (HL)", "");
        break;
      }
      case 0x35: {
        uint8_t value = cpu.memory[cpu.HL];
        uint8_t pre = value;
        value--;
        cpu.Flag |= FLAG_N;
        if (value == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        cpu.memory[cpu.HL] = value;
        printDebug("DEC (HL)", "");
        break;
      }
      case 0x36: {
        uint8_t value = cpu.memory[cpu.PC++];
        cpu.memory[cpu.HL] = value;
        printDebug("LD (HL), u8", "");
        break;
      }
      case 0x37:
        cpu.Flag |= FLAG_C;
        cpu.Flag &= ~(FLAG_N | FLAG_H);
        printDebug("SCF", "");
        break;
      case 0x38: {
        int8_t offset = cpu.memory[cpu.PC++];
        if (cpu.Flag & FLAG_C)
          cpu.PC += offset;
        printDebug("JR C, i8", "");
        break;
      }
      case 0x39: {
        uint32_t result = cpu.HL + cpu.SP;
        cpu.Flag &= ~FLAG_N;
        if (((cpu.HL & 0x0FFF) + (cpu.SP & 0x0FFF)) > 0x0FFF)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (result > 0xFFFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.HL = result & 0xFFFF;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("ADD HL, SP", "");
        break;
      }
      case 0x3A:
        cpu.A = cpu.memory[cpu.HL];
        cpu.HL--;
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        printDebug("LD A, (HL-)", "");
        break;
      case 0x3B:
        cpu.SP--;
        printDebug("DEC SP", "");
        break;
      case 0x3C: {
        uint8_t pre = cpu.A;
        cpu.A++;
        cpu.Flag &= ~FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("INC A", "");
        break;
      }
      case 0x3D: {
        uint8_t pre = cpu.A;
        cpu.A--;
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        if ((pre & 0x0F) == 0x00)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;
        printDebug("DEC A", "");
        break;
      }
      case 0x3E:
        cpu.A = cpu.memory[cpu.PC++];
        printDebug("LD A, u8", "");
        break;
      case 0x3F:
        if (cpu.Flag & FLAG_C)
          cpu.Flag &= ~FLAG_C;
        else
          cpu.Flag |= FLAG_C;
        cpu.Flag &= ~(FLAG_N | FLAG_H);
        printDebug("CCF", "");
        break;
      case 0x40:
        printDebug("LD B, B", "");
        break;
      case 0x41:
        cpu.B = cpu.C;
        printDebug("LD B, C", "");
        break;
      case 0x42:
        cpu.B = cpu.D;
        printDebug("LD B, D", "");
        break;
      case 0x43:
        cpu.B = cpu.E;
        printDebug("LD B, E", "");
        break;
      case 0x44:
        cpu.B = cpu.H;
        printDebug("LD B, H", "");
        break;
      case 0x45:
        cpu.B = cpu.L;
        printDebug("LD B, L", "");
        break;
      case 0x46:
        cpu.B = cpu.memory[cpu.HL];
        printDebug("LD B, (HL)", "");
        break;
      case 0x47:
        cpu.B = cpu.A;
        printDebug("LD B, A", "");
        break;
      case 0x48:
        cpu.C = cpu.B;
        printDebug("LD C, B", "");
        break;
      case 0x49:
        printDebug("LD C, C", "");
        break;
      case 0x4A:
        cpu.C = cpu.D;
        printDebug("LD C, D", "");
        break;
      case 0x4B:
        cpu.C = cpu.E;
        printDebug("LD C, E", "");
        break;
      case 0x4C:
        cpu.C = cpu.H;
        printDebug("LD C, H", "");
        break;
      case 0x4D:
        cpu.C = cpu.L;
        printDebug("LD C, L", "");
        break;
      case 0x4E:
        cpu.C = cpu.memory[cpu.HL];
        printDebug("LD C, (HL)", "");
        break;
      case 0x4F:
        cpu.C = cpu.A;
        printDebug("LD C, A", "");
        break;
      case 0x50:
        cpu.D = cpu.B;
        printDebug("LD D, B", "");
        break;
      case 0x51:
        cpu.D = cpu.C;
        printDebug("LD D, C", "");
        break;
      case 0x52:
        printDebug("LD D, D", "");
        break;
      case 0x53:
        cpu.D = cpu.E;
        printDebug("LD D, E", "");
        break;
      case 0x54:
        cpu.D = cpu.H;
        printDebug("LD D, H", "");
        break;
      case 0x55:
        cpu.D = cpu.L;
        printDebug("LD D, L", "");
        break;
      case 0x56:
        cpu.D = cpu.memory[cpu.HL];
        printDebug("LD D, (HL)", "");
        break;
      case 0x57:
        cpu.D = cpu.A;
        printDebug("LD D, A", "");
        break;
      case 0x58:
        cpu.E = cpu.B;
        printDebug("LD E, B", "");
        break;
      case 0x59:
        cpu.E = cpu.C;
        printDebug("LD E, C", "");
        break;
      case 0x5A:
        cpu.E = cpu.D;
        printDebug("LD E, D", "");
        break;
      case 0x5B:
        printDebug("LD E, E", "");
        break;
      case 0x5C:
        cpu.E = cpu.H;
        printDebug("LD E, H", "");
        break;
      case 0x5D:
        cpu.E = cpu.L;
        printDebug("LD E, L", "");
        break;
      case 0x5E:
        cpu.E = cpu.memory[cpu.HL];
        printDebug("LD E, (HL)", "");
        break;
      case 0x5F:
        cpu.E = cpu.A;
        printDebug("LD E, A", "");
        break;
      case 0x60:
        cpu.H = cpu.B;
        printDebug("LD H, B", "");
        break;
      case 0x61:
        cpu.H = cpu.C;
        printDebug("LD H, C", "");
        break;
      case 0x62:
        cpu.H = cpu.D;
        printDebug("LD H, D", "");
        break;
      case 0x63:
        cpu.H = cpu.E;
        printDebug("LD H, E", "");
        break;
      case 0x64:
        printDebug("LD H, H", "");
        break;
      case 0x65:
        cpu.H = cpu.L;
        printDebug("LD H, L", "");
        break;
      case 0x66:
        cpu.H = cpu.memory[cpu.HL];
        printDebug("LD H, (HL)", "");
        break;
      case 0x67:
        cpu.H = cpu.A;
        printDebug("LD H, A", "");
        break;
      case 0x68:
        cpu.L = cpu.B;
        printDebug("LD L, B", "");
        break;
      case 0x69:
        cpu.L = cpu.C;
        printDebug("LD L, C", "");
        break;
      case 0x6A:
        cpu.L = cpu.D;
        printDebug("LD L, D", "");
        break;
      case 0x6B:
        cpu.L = cpu.E;
        printDebug("LD L, E", "");
        break;
      case 0x6C:
        cpu.L = cpu.H;
        printDebug("LD L, H", "");
        break;
      case 0x6D:
        printDebug("LD L, L", "");
        break;
      case 0x6E:
        cpu.L = cpu.memory[cpu.HL];
        printDebug("LD L, (HL)", "");
        break;
      case 0x6F:
        cpu.L = cpu.A;
        printDebug("LD L, A", "");
        break;
      case 0x70:
        cpu.memory[cpu.HL] = cpu.B;
        printDebug("LD (HL), B", "");
        break;
      case 0x71:
        cpu.memory[cpu.HL] = cpu.C;
        printDebug("LD (HL), C", "");
        break;
      case 0x72:
        cpu.memory[cpu.HL] = cpu.D;
        printDebug("LD (HL), D", "");
        break;
      case 0x73:
        cpu.memory[cpu.HL] = cpu.E;
        printDebug("LD (HL), E", "");
        break;
      case 0x74:
        cpu.memory[cpu.HL] = cpu.H;
        printDebug("LD (HL), H", "");
        break;
      case 0x75:
        cpu.memory[cpu.HL] = cpu.L;
        printDebug("LD (HL), L", "");
        break;
      case 0x76:
        halted = true;
        printDebug("HALT", "");
        break;
      case 0x77:
        cpu.memory[cpu.HL] = cpu.A;
        printDebug("LD (HL), A", "");
        break;
      case 0x78:
        cpu.A = cpu.B;
        printDebug("LD A, B", "");
        break;
      case 0x79:
        cpu.A = cpu.C;
        printDebug("LD A, C", "");
        break;
      case 0x7A:
        cpu.A = cpu.D;
        printDebug("LD A, D", "");
        break;
      case 0x7B:
        cpu.A = cpu.E;
        printDebug("LD A, E", "");
        break;
      case 0x7C:
        cpu.A = cpu.H;
        printDebug("LD A, H", "");
        break;
      case 0x7D:
        cpu.A = cpu.L;
        printDebug("LD A, L", "");
        break;
      case 0x7E:
        cpu.A = cpu.memory[cpu.HL];
        printDebug("LD A, (HL)", "");
        break;
      case 0x7F:
        printDebug("LD A, A", "");
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
