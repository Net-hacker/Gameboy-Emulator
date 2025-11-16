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
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 * @param size Die Größe der .gb Datei.
 */
void Debugging(CPU cpu, long size)
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
      printDebug("Groesse: ", "%d", sizeof(cpu.memory));
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
      FILE *rom_d = fopen("Rom.txt", "w");
      printDebug("Groesse: ", "%d", size);
      fprintf(rom_d, "  ");
      for (size_t i = 0; i < size; i++) {
        fprintf(rom_d, "%02x ", cpu.rom[i]);
        if ((i + 1) % 16 == 0)
          fprintf(rom_d, "  %06X\n", i);
        if ((i + 1) % 8 == 0)
          fprintf(rom_d, "  ");
      }
      fclose(rom_d);
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
 * Gibt den uint8_t Wert vom ROM oder RAM zurück.
 *
 * Hier wird ein Wert aus entweder dem RAM oder der ROM an einer Addresse ausgelesen.
 *
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 * @param addr Die Addresse, des Wertes.
 */
uint8_t cpu_read8(CPU cpu, uint16_t addr)
{
  if (addr < 0x8000)
    return cpu.rom[addr];
  else
    return cpu.memory[addr];
}

/**
 * Schreib einen uint8_t Wert in den RAM.
 *
 * Hier wird ein Wert in den RAM an einer bestimmten Addresse gespeichert.
 *
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 * @param addr Die Addresse, des Wertes.
 * @param val Der Wert.
 */
void cpu_write8(CPU cpu, uint16_t addr, uint8_t val)
{
  if (addr >= 0x8000)
    cpu.memory[addr] = val;
}

/**
 * Der Interpreter von OpCodes.
 *
 * Hier werden die eingelesen OpCodes interpretiert und ausgeführt.
 *
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 * @param debug Wenn true, springt am Ende des Programms zum DebugMenu.
 * @param size Die Größe der .gb Datei.
 */
void Run(CPU cpu, bool debug, long size)
{
  uint8_t opcode = cpu.rom[cpu.PC];
  bool halted = false;
  bool jumped = false;
  uint8_t count = 0;
  uint8_t prePC;

  while (!WindowShouldClose() && count <= 100 && !halted) {
    printf("%02X\n", cpu.rom[cpu.PC]);
    switch (opcode) {
      case 0x00:
        printDebug("NOP", "");
        break;
      case 0x01: {
        cpu.BC = cpu_read8(cpu, cpu.PC + 1) << 8 | cpu_read8(cpu, cpu.PC + 2);
        cpu.B = (cpu.BC >> 8) & 0xFF;
        cpu.C = cpu.BC & 0xFF;
        cpu.PC += 2;
        printDebug("LD BC, u16", "");
        break;
      }
      case 0x02: {
        cpu_write8(cpu, cpu.BC, cpu.A);
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
        cpu.B = cpu_read8(cpu, cpu.PC++);
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
        uint16_t address = cpu_read8(cpu, cpu.PC + 1) | cpu_read8(cpu, cpu.PC + 2);
        cpu_write8(cpu, address, cpu.SP & 0xFF);
        cpu_write8(cpu, address + 1, (cpu.SP >> 8) & 0xFF);
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
        cpu.A = cpu_read8(cpu, cpu.BC);
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
        cpu.C = cpu_read8(cpu, cpu.PC++);
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
        cpu.DE = (cpu_read8(cpu, cpu.PC + 2) << 8) | cpu_read8(cpu, cpu.PC + 1);
        cpu.D = (cpu.DE >> 8) & 0xFF;
        cpu.E = cpu.DE & 0xFF;
        cpu.PC += 2;
        printDebug("LD DE, u16", "");
        break;
      }
      case 0x12: {
        cpu_write8(cpu, cpu.DE, cpu.A);
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
        cpu.D = cpu_read8(cpu, cpu.PC++);
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
        int8_t offset = cpu_read8(cpu, cpu.PC++);
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
        cpu.A = cpu_read8(cpu, cpu.DE);
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
        cpu.E = cpu_read8(cpu, cpu.PC++);
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
        int8_t offset = cpu_read8(cpu, cpu.PC++);
        if (!(cpu.Flag & FLAG_Z))
          cpu.PC += offset;
        printDebug("JR NZ, i8", "");
        break;
      }
      case 0x21:
        cpu.HL = (cpu_read8(cpu, cpu.PC + 2) << 8) | cpu_read8(cpu, cpu.PC + 1);
        cpu.H = (cpu.HL >> 8) & 0xFF;
        cpu.L = cpu.HL & 0xFF;
        cpu.PC += 2;
        printDebug("LD HL, u16", "");
        break;
      case 0x22:
        cpu_write8(cpu, cpu.HL, cpu.A);
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
        cpu.H = cpu_read8(cpu, cpu.PC++);
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
        int8_t offset = cpu_read8(cpu, cpu.PC++);
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
        cpu.A = cpu_read8(cpu, cpu.HL);
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
        cpu.L = cpu_read8(cpu, cpu.PC++);
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
        int8_t offset = cpu_read8(cpu, cpu.PC++);
        if (!(cpu.Flag & FLAG_C))
          cpu.PC += offset;
        printDebug("JR NC, i8", "");
        break;
      }
      case 0x31:
        cpu.SP = (cpu_read8(cpu, cpu.PC + 2) << 8) | cpu_read8(cpu, cpu.PC + 1);
        cpu.PC += 2;
        printDebug("LD SP, u16", "");
        break;
      case 0x32:
        cpu_write8(cpu, cpu.HL, cpu.A);
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
        uint8_t value = cpu_read8(cpu, cpu.HL);
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
        cpu_write8(cpu, cpu.HL, value);
        printDebug("INC (HL)", "");
        break;
      }
      case 0x35: {
        uint8_t value = cpu_read8(cpu, cpu.HL);
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
        cpu_write8(cpu, cpu.HL, value);
        printDebug("DEC (HL)", "");
        break;
      }
      case 0x36: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        cpu_write8(cpu, cpu.HL, value);
        printDebug("LD (HL), u8", "");
        break;
      }
      case 0x37:
        cpu.Flag |= FLAG_C;
        cpu.Flag &= ~(FLAG_N | FLAG_H);
        printDebug("SCF", "");
        break;
      case 0x38: {
        int8_t offset = cpu_read8(cpu, cpu.PC++);
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
        cpu.A = cpu_read8(cpu, cpu.HL);
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
        cpu.A = cpu_read8(cpu, cpu.PC++);
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
        cpu.B = cpu_read8(cpu, cpu.HL);
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
        cpu.C = cpu_read8(cpu, cpu.HL);
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
        cpu.D = cpu_read8(cpu, cpu.HL);
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
        cpu.E = cpu_read8(cpu, cpu.HL);
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
        cpu.H = cpu_read8(cpu, cpu.HL);
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
        cpu.L = cpu_read8(cpu, cpu.HL);
        printDebug("LD L, (HL)", "");
        break;
      case 0x6F:
        cpu.L = cpu.A;
        printDebug("LD L, A", "");
        break;
      case 0x70:
        cpu_write8(cpu, cpu.HL, cpu.B);
        printDebug("LD (HL), B", "");
        break;
      case 0x71:
        cpu_write8(cpu, cpu.HL, cpu.C);
        printDebug("LD (HL), C", "");
        break;
      case 0x72:
        cpu_write8(cpu, cpu.HL, cpu.D);
        printDebug("LD (HL), D", "");
        break;
      case 0x73:
        cpu_write8(cpu, cpu.HL, cpu.E);
        printDebug("LD (HL), E", "");
        break;
      case 0x74:
        cpu_write8(cpu, cpu.HL, cpu.H);
        printDebug("LD (HL), H", "");
        break;
      case 0x75:
        cpu_write8(cpu, cpu.HL, cpu.L);
        printDebug("LD (HL), L", "");
        break;
      case 0x76:
        halted = true;
        printDebug("HALT", "");
        break;
      case 0x77:
        cpu_write8(cpu, cpu.HL, cpu.A);
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
        cpu.A = cpu_read8(cpu, cpu.HL);
        printDebug("LD A, (HL)", "");
        break;
      case 0x7F:
        printDebug("LD A, A", "");
        break;
      case 0x80: {
        uint8_t a = cpu.A;
        uint8_t b = cpu.B;
        uint16_t result = a + b;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (b & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, B", "");
        break;
      }
      case 0x81: {
        uint8_t a = cpu.A;
        uint8_t c = cpu.C;
        uint16_t result = a + c;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (c & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, C", "");
        break;
      }
      case 0x82: {
        uint8_t a = cpu.A;
        uint8_t d = cpu.D;
        uint16_t result = a + d;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (d & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, D", "");
        break;
      }
      case 0x83: {
        uint8_t a = cpu.A;
        uint8_t e = cpu.E;
        uint16_t result = a + e;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (e & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, E", "");
        break;
      }
      case 0x84: {
        uint8_t a = cpu.A;
        uint8_t h = cpu.H;
        uint16_t result = a + h;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (h & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, H", "");
        break;
      }
      case 0x85: {
        uint8_t a = cpu.A;
        uint8_t l = cpu.L;
        uint16_t result = a + l;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (l & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, L", "");
        break;
      }
      case 0x86: {
        uint8_t a = cpu.A;
        uint8_t value = cpu_read8(cpu, cpu.HL);
        uint16_t result = a + value;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (value & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, (HL)", "");
        break;
      }
      case 0x87: {
        uint8_t a = cpu.A;
        uint16_t result = a + a;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (a & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADD A, A", "");
        break;
      }
      case 0x88: {
        uint8_t a = cpu.A;
        uint8_t b = cpu.B;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + b + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, B", "");
        break;
      }
      case 0x89: {
        uint8_t a = cpu.A;
        uint8_t c = cpu.C;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + c + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (c & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, C", "");
        break;
      }
      case 0x8A: {
        uint8_t a = cpu.A;
        uint8_t d = cpu.D;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + d + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (d & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, D", "");
        break;
      }
      case 0x8B: {
        uint8_t a = cpu.A;
        uint8_t e = cpu.E;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + e + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (e & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, E", "");
        break;
      }
      case 0x8C: {
        uint8_t a = cpu.A;
        uint8_t h = cpu.H;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + h + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (h & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, H", "");
        break;
      }
      case 0x8D: {
        uint8_t a = cpu.A;
        uint8_t l = cpu.L;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + l + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (l & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, L", "");
        break;
      }
      case 0x8E: {
        uint8_t a = cpu.A;
        uint8_t value = cpu_read8(cpu, cpu.HL);
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + value + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (value & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, (HL)", "");
        break;
      }
      case 0x8F: {
        uint8_t a = cpu.A;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a + a + carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_Z | FLAG_C);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if (((a & 0x0F) + (a & 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        printDebug("ADC A, A", "");
        break;
      }
      case 0x90: {
        uint8_t a = cpu.A;
        uint8_t b = cpu.B;
        uint16_t result = a - b;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (b & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < b)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, B", "");
        break;
      }
      case 0x91: {
        uint8_t a = cpu.A;
        uint8_t c = cpu.C;
        uint16_t result = a - c;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (c & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < c)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, C", "");
        break;
      }
      case 0x92: {
        uint8_t a = cpu.A;
        uint8_t d = cpu.D;
        uint16_t result = a - d;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (d & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < d)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, D", "");
        break;
      }
      case 0x93: {
        uint8_t a = cpu.A;
        uint8_t e = cpu.E;
        uint16_t result = a - e;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (e & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < e)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, E", "");
        break;
      }
      case 0x94: {
        uint8_t a = cpu.A;
        uint8_t h = cpu.H;
        uint16_t result = a - h;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (h & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < h)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, H", "");
        break;
      }
      case 0x95: {
        uint8_t a = cpu.A;
        uint8_t l = cpu.L;
        uint16_t result = a - l;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (l & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < l)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, L", "");
        break;
      }
      case 0x96: {
        uint8_t a = cpu.A;
        uint8_t value = cpu_read8(cpu, cpu.HL);
        uint16_t result = a - value;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < (value & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < value)
          cpu.Flag |= FLAG_C;
        printDebug("SUB A, (HL)", "");
        break;
      }
      case 0x97:
        cpu.A = 0;
        cpu.Flag &= ~(FLAG_H | FLAG_C);
        cpu.Flag |= FLAG_N | FLAG_Z;
        printDebug("SUB A, A", "");
        break;
      case 0x98: {
        uint8_t a = cpu.A;
        uint8_t b = cpu.B;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - b - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((b & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (b + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, B", "");
        break;
      }
      case 0x99: {
        uint8_t a = cpu.A;
        uint8_t c = cpu.C;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - c - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((c & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (c + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, C", "");
        break;
      }
      case 0x9A: {
        uint8_t a = cpu.A;
        uint8_t d = cpu.D;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - d - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((d & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (d + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, D", "");
        break;
      }
      case 0x9B: {
        uint8_t a = cpu.A;
        uint8_t e = cpu.E;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - e - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((e & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (e + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, E", "");
        break;
      }
      case 0x9C: {
        uint8_t a = cpu.A;
        uint8_t h = cpu.H;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - h - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((h & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (h + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, H", "");
        break;
      }
      case 0x9D: {
        uint8_t a = cpu.A;
        uint8_t l = cpu.L;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - l - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((l & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (l + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, L", "");
        break;
      }
      case 0x9E: {
        uint8_t a = cpu.A;
        uint8_t value = cpu_read8(cpu, cpu.HL);
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - value - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((value & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (a < (value + carry))
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, (HL)", "");
        break;
      }
      case 0x9F: {
        uint8_t a = cpu.A;
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = a - a - carry;
        cpu.A = result & 0xFF;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;

        if ((a & 0x0F) < ((a & 0x0F) + carry))
          cpu.Flag |= FLAG_H;

        if (carry)
          cpu.Flag |= FLAG_C;
        printDebug("SBC A, A", "");
        break;
      }
      case 0xA0:
        cpu.A &= cpu.B;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, B", "");
        break;
      case 0xA1:
        cpu.A &= cpu.C;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, C", "");
        break;
      case 0xA2:
        cpu.A &= cpu.D;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, D", "");
        break;
      case 0xA3:
        cpu.A &= cpu.E;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, E", "");
        break;
      case 0xA4:
        cpu.A &= cpu.H;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, H", "");
        break;
      case 0xA5:
        cpu.A &= cpu.L;
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, L", "");
        break;
      case 0xA6:
        cpu.A &= cpu_read8(cpu, cpu.HL);
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, (HL)", "");
        break;
      case 0xA7:
        cpu.Flag &= ~(FLAG_N | FLAG_C | FLAG_Z);
        cpu.Flag |= FLAG_H;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("AND A, A", "");
        break;
      case 0xA8:
        cpu.A ^= cpu.B;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, B", "");
        break;
      case 0xA9:
        cpu.A ^= cpu.C;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, C", "");
        break;
      case 0xAA:
        cpu.A ^= cpu.D;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, D", "");
        break;
      case 0xAB:
        cpu.A ^= cpu.E;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, E", "");
        break;
      case 0xAC:
        cpu.A ^= cpu.H;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, H", "");
        break;
      case 0xAD:
        cpu.A ^= cpu.L;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, L", "");
        break;
      case 0xAE:
        cpu.A ^= cpu_read8(cpu, cpu.HL);
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("XOR A, (HL)", "");
        break;
      case 0xAF:
        cpu.A = 0;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C);
        cpu.Flag |= FLAG_Z;
        printDebug("XOR A, A", "");
        break;
      case 0xB0:
        cpu.A |= cpu.B;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, B", "");
        break;
      case 0xB1:
        cpu.A |= cpu.C;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, C", "");
        break;
      case 0xB2:
        cpu.A |= cpu.D;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, C", "");
        break;
      case 0xB3:
        cpu.A |= cpu.E;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, D", "");
        break;
      case 0xB4:
        cpu.A |= cpu.H;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, H", "");
        break;
      case 0xB5:
        cpu.A |= cpu.L;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, L", "");
        break;
      case 0xB6:
        cpu.A |= cpu_read8(cpu, cpu.HL);
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, (HL)", "");
        break;
      case 0xB7:
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C | FLAG_Z);
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("OR A, A", "");
        break;
      case 0xB8: {
        uint8_t a = cpu.A;
        uint8_t b = cpu.B;
        uint16_t result = a - b;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (b & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < b)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, B", "");
        break;
      }
      case 0xB9: {
        uint8_t a = cpu.A;
        uint8_t c = cpu.C;
        uint16_t result = a - c;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (c & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < c)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, C", "");
        break;
      }
      case 0xBA: {
        uint8_t a = cpu.A;
        uint8_t d = cpu.D;
        uint16_t result = a - d;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (d & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < d)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, D", "");
        break;
      }
      case 0xBB: {
        uint8_t a = cpu.A;
        uint8_t e = cpu.E;
        uint16_t result = a - e;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (e & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < e)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, E", "");
        break;
      }
      case 0xBC: {
        uint8_t a = cpu.A;
        uint8_t h = cpu.H;
        uint16_t result = a - h;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (h & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < h)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, H", "");
        break;
      }
      case 0xBD: {
        uint8_t a = cpu.A;
        uint8_t l = cpu.L;
        uint16_t result = a - l;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (l & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < l)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, L", "");
        break;
      }
      case 0xBE: {
        uint8_t a = cpu.A;
        uint8_t value = cpu_read8(cpu, cpu.HL);
        uint16_t result = a - value;
        cpu.Flag &= ~(FLAG_H | FLAG_Z | FLAG_C);
        cpu.Flag |= FLAG_N;
        if ((a & 0x0F) < (value & 0x0F))
          cpu.Flag |= FLAG_H;

        if (a < value)
          cpu.Flag |= FLAG_C;

        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        printDebug("CP A, (HL)", "");
        break;
      }
      case 0xBF: {
        cpu.Flag &= ~(FLAG_H | FLAG_C);
        cpu.Flag |= (FLAG_N | FLAG_Z);
        printDebug("CP A, A", "");
        break;
      }
      case 0xC0: {
        if (!(cpu.Flag & FLAG_Z)) {
          uint8_t lo = cpu_read8(cpu, cpu.SP++);
          uint8_t hi = cpu_read8(cpu, cpu.SP++);
          cpu.PC = (hi << 8) | lo;
          jumped = true;
        }
        printDebug("RET NZ", "");
        break;
      }
      case 0xC1: {
        cpu.B = cpu_read8(cpu, cpu.SP++);
        cpu.C = cpu_read8(cpu, cpu.SP++);
        cpu.BC = (cpu.B << 8) | cpu.C;
        printDebug("POP BC", "");
        break;
      }
      case 0xC2: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (!(cpu.Flag & FLAG_Z)) {
            cpu.PC = addr;
            jumped = true;
        }
        printDebug("JP NZ, u16", "");
        break;
      }
      case 0xC3: {
        uint8_t lo = cpu_read8(cpu, cpu.PC + 1);
        uint8_t hi = cpu_read8(cpu, cpu.PC + 2);
        uint16_t addr = (hi << 8) | lo;
        cpu.PC = addr;
        jumped = true;
        printDebug("JP u16", "");
        break;
      }
      case 0xC4: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (!(cpu.Flag & FLAG_Z)) {
          uint16_t return_addr = cpu.PC++;
          cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
          cpu_write8(cpu, --cpu.SP,return_addr & 0xFF);
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("CALL NZ, u16", "");
        break;
      }
      case 0xC5: {
        uint16_t value = cpu.BC;
        cpu_write8(cpu, --cpu.SP, (value >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, value & 0xFF);
      }
      case 0xC6: {
        uint8_t val = cpu_read8(cpu, cpu.PC++);
        uint16_t result = cpu.A + val;
        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;

        if ((cpu.A & 0x0F) + (val & 0x0F) > 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.A = (uint8_t) result;
        printDebug("ADD A, u8", "");
        break;
      }
      case 0xC7: {
        uint16_t return_addr = cpu.PC + 1;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0000;
        jumped = true;
        printDebug("RST 00h", "");
        break;
      }
      case 0xC8: {
        if (cpu.Flag & FLAG_Z) {
          uint8_t lo = cpu_read8(cpu, cpu.SP++);
          uint8_t hi = cpu_read8(cpu, cpu.SP++);
          cpu.PC = (hi << 8) | lo;
          jumped = true;
        }
        printDebug("RET Z", "");
        break;
      }
      case 0xC9: {
        uint8_t lo = cpu_read8(cpu, cpu.SP++);
        uint8_t hi = cpu_read8(cpu, cpu.SP++);
        cpu.PC = (hi << 8) | lo;
        jumped = true;
        printDebug("RET", "");
        break;
      }
      case 0xCA: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (cpu.Flag & FLAG_Z) {
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("JP Z, u16", "");
        break;
      }
      case 0xCB: {
        uint8_t cbcode = cpu_read8(cpu, cpu.PC++);
        //Executing CB OpCodes
      }
      case 0xCC: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (cpu.Flag & FLAG_Z) {
          uint16_t return_addr = cpu.PC++;
          cpu_write8(cpu, --cpu.SP, (return_addr >> 8) | 0xFF);
          cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("CALL Z, u16", "");
        break;
      }
      case 0xCD: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) | 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = addr;
        jumped = true;
        printDebug("CALL u16", "");
        break;
      }
      case 0xCE: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = cpu.A + value + carry;
        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;
        if (((cpu.A & 0x0F) + (value + 0x0F) + carry) > 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (result > 0xFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.A = (uint8_t) result;
        printDebug("ADC A, u8", "");
        break;
      }
      case 0xCF: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0008;
        jumped = true;
        printDebug("RST 08h", "");
        break;
      }
      case 0xD0: {
        if (!(cpu.Flag & FLAG_C)) {
          uint8_t lo = cpu_read8(cpu, cpu.SP++);
          uint8_t hi = cpu_read8(cpu, cpu.SP++);
          cpu.PC = (hi << 8) | lo;
          jumped = true;
        }
        printDebug("RET NC", "");
        break;
      }
      case 0xD1: {
        cpu.D = cpu_read8(cpu, cpu.SP++);
        cpu.E = cpu_read8(cpu, cpu.SP++);
        cpu.DE = (cpu.D << 8) | cpu.E;
        printDebug("POP DE", "");
        break;
      }
      case 0xD2: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (!(cpu.Flag & FLAG_C)) {
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("JP NC, u16", "");
        break;
      }
      case 0xD4: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (!(cpu.Flag & FLAG_C)) {
          uint16_t return_addr = cpu.PC++;
          cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
          cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("CALL NC, u16", "");
        break;
      }
      case 0xD5: {
        uint16_t value = cpu.DE;
        cpu_write8(cpu, --cpu.SP, (value >> 8) | 0xFF);
        cpu_write8(cpu, --cpu.SP, value & 0xFF);
        printDebug("PUSH DE", "");
        break;
      }
      case 0xD6: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        uint16_t result = cpu.A - value;
        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;

        if ((cpu.A & 0x0F) < (value & 0x0F))
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (cpu.A < 0xFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.A = (uint8_t) result;
        printDebug("SUB A, u8", "");
        break;
      }
      case 0xD7: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0010;
        jumped = true;
        printDebug("RST 10h", "");
        break;
      }
      case 0xD8: {
        if (cpu.Flag & FLAG_C) {
          uint8_t lo = cpu_read8(cpu, cpu.SP++);
          uint8_t hi = cpu_read8(cpu, cpu.SP++);
          cpu.PC = (hi << 8) | lo;
          jumped = true;
        }
        printDebug("RET C", "");
        break;
      }
      case 0xD9: {
        uint8_t lo = cpu_read8(cpu, cpu.SP++);
        uint8_t hi = cpu_read8(cpu, cpu.SP++);
        cpu.PC = (hi << 8) | lo;
        jumped = true;
        cpu.IME = true;
        printDebug("RETI", "");
        break;
      }
      case 0xDA: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (cpu.Flag & FLAG_C) {
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("JP C, u16", "");
        break;
      }
      case 0xDC: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        if (cpu.Flag & FLAG_C) {
          uint16_t return_addr = cpu.PC++;
          cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
          cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
          cpu.PC = addr;
          jumped = true;
        }
        printDebug("CALL C, u16", "");
        break;
      }
      case 0xDE: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        uint8_t carry = (cpu.Flag & FLAG_C) ? 1 : 0;
        uint16_t result = cpu.A - value - carry;
        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag |= FLAG_N;
        if ((cpu.A & 0x0F) < ((value & 0x0F) + carry))
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (result > 0xFF)
          cpu.Flag &= ~FLAG_C;
        if (cpu.A < value + carry)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.A = (uint8_t) result;
        printDebug("SBC A, u8", "");
        break;
      }
      case 0xDF: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0018;
        jumped = true;
        printDebug("RST 18h", "");
        break;
      }
      case 0xE0: {
        uint8_t offset = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = 0xFF00 + offset;
        cpu_write8(cpu, addr, cpu.A);
        printDebug("LD (FF00+u8), A", "");
        break;
      }
      case 0xE1: {
        cpu.H = cpu_read8(cpu, cpu.SP++);
        cpu.L = cpu_read8(cpu, cpu.SP++);
        cpu.HL = (cpu.H << 8) | cpu.L;
        printDebug("POP HL", "");
        break;
      }
      case 0xE2: {
        uint16_t addr = 0xFF00 + cpu.C;
        cpu_write8(cpu, addr, cpu.A);
        printDebug("LD (FF00+C), A", "");
        break;
      }
      case 0xE5: {
        cpu_write8(cpu, --cpu.SP, cpu.H);
        cpu_write8(cpu, --cpu.SP, cpu.L);
        printDebug("PUSH HL", "");
        break;
      }
      case 0xE6: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        cpu.A &= value;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;

        cpu.Flag &= ~FLAG_N;
        cpu.Flag |= FLAG_H;
        cpu.Flag &= ~FLAG_C;
        printDebug("AND A, u8", "");
        break;
      }
      case 0xE7: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0020;
        jumped = true;
        printDebug("RST 20h", "");
        break;
      }
      case 0xE8: {
        int8_t imm = (int8_t) cpu_read8(cpu, cpu.PC++);
        uint16_t sp = cpu.SP;
        cpu.Flag &= ~(FLAG_Z | FLAG_N);
        if (((sp & 0x0F) + (imm & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (((sp & 0xFF) + (imm & 0xFF)) > 0xFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.SP = sp + imm;
        printDebug("ADD SP, i8", "");
        break;
      }
      case 0xE9: {
        cpu.PC = cpu.HL;
        jumped = true;
        printDebug("JP HL", "");
        break;
      }
      case 0xEA: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        cpu_write8(cpu, addr, cpu.A);
        printDebug("LD (u16), A", "");
        break;
      }
      case 0xEE: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        cpu.A ^= value;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C);
        printDebug("XOR A, u8", "");
        break;
      }
      case 0xEF: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0028;
        jumped = true;
        printDebug("RST 28h", "");
        break;
      }
      case 0xF0: {
        uint8_t offset = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = 0xFF00 + offset;
        cpu.A = cpu_read8(cpu, addr);
        printDebug("LD A, (FF00+u8)", "");
        break;
      }
      case 0xF1: {
        cpu.A = cpu_read8(cpu, cpu.SP++);
        cpu.Flag = cpu_read8(cpu, cpu.SP++) & 0xF0;
        printDebug("POP AF", "");
        break;
      }
      case 0xF2: {
        uint16_t addr = 0xFF00 + cpu.C;
        cpu.A = cpu_read8(cpu, addr);
        printDebug("LD A, (FF00+C)", "");
        break;
      }
      case 0xF3: {
        cpu.IME = false;
        printDebug("DI", "");
        break;
      }
      case 0xF5: {
        cpu_write8(cpu, --cpu.SP, cpu.A);
        cpu_write8(cpu, --cpu.SP, cpu.Flag & 0xF0);
        printDebug("POP AF", "");
        break;
      }
      case 0xF6: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        cpu.A |= value;
        if (cpu.A == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;
        cpu.Flag &= ~(FLAG_N | FLAG_H | FLAG_C);
        printDebug("OR A, u8", "");
        break;
      }
      case 0xF7: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0030;
        jumped = true;
        printDebug("RST 30h", "");
        break;
      }
      case 0xF8: {
        int8_t imm = (int8_t) cpu_read8(cpu, cpu.PC++);
        uint16_t sp = cpu.SP;
        uint16_t result = sp + imm;
        cpu.Flag &= ~FLAG_Z;
        cpu.Flag &= ~FLAG_N;
        if (((sp & 0x0F) + (imm & 0x0F)) > 0x0F)
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (((sp & 0xFF) + (imm & 0xFF)) > 0xFF)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        cpu.HL = result;
        printDebug("LD HL, SP+i8", "");
        break;
      }
      case 0xF9: {
        cpu.SP = cpu.HL;
        printDebug("LD SP, HL", "");
        break;
      }
      case 0xFA: {
        uint8_t lo = cpu_read8(cpu, cpu.PC++);
        uint8_t hi = cpu_read8(cpu, cpu.PC++);
        uint16_t addr = (hi << 8) | lo;
        cpu.A = cpu_read8(cpu, addr);
        printDebug("LD A, (u16)", "");
        break;
      }
      case 0xFB: {
        cpu.EI_pending = true;
        printDebug("EI", "");
      }
      case 0xFE: {
        uint8_t value = cpu_read8(cpu, cpu.PC++);
        uint16_t result = cpu.A - value;
        if ((result & 0xFF) == 0)
          cpu.Flag |= FLAG_Z;
        else
          cpu.Flag &= ~FLAG_Z;
        cpu.Flag |= FLAG_N;

        if ((cpu.A & 0x0F) < (value & 0x0F))
          cpu.Flag |= FLAG_H;
        else
          cpu.Flag &= ~FLAG_H;

        if (cpu.A < value)
          cpu.Flag |= FLAG_C;
        else
          cpu.Flag &= ~FLAG_C;
        printDebug("CP A, u8", "");
        break;
      }
      case 0xFF: {
        uint16_t return_addr = cpu.PC++;
        cpu_write8(cpu, --cpu.SP, (return_addr >> 8) & 0xFF);
        cpu_write8(cpu, --cpu.SP, return_addr & 0xFF);
        cpu.PC = 0x0038;
        jumped = true;
        printDebug("RST 38h", "");
        break;
      }
      default:
        printError("OpCode not found!", "");
        break;
    }
    if (!(jumped))
      cpu.PC++;
    if (cpu.EI_pending) {
      prePC = cpu.PC;
      if (prePC != cpu.PC) {
        cpu.IME = true;
        cpu.EI_pending = false;
      }
    }
    jumped = false;
    opcode = cpu.rom[cpu.PC];
    count++;
  }

  if (debug) {
    Debugging(cpu, size);
  }
}
