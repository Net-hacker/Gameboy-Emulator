#include <cpu.h>
#include <debug.h>

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

  while (cpu.PC <= 20) {
    printf("%02X\n", rom[cpu.PC]);
    switch (opcode) {
      case 0x00:
        printDebug("NOP", "");
        break;
      case 0x01:
        cpu.B = (rom[cpu.PC++] > 8) & 0xFF;
        cpu.PC += 2;
        cpu.C = rom[cpu.PC++] & 0xFF;
        printDebug("LD BC, u16", "");
        break;
      case 0x02:
        cpu.memory[cpu.PC++] = cpu.A;
        cpu.PC++;
        printDebug("LD (BC), A", "");
        break;
      case 0x03: {
        uint16_t BC = (cpu.B << 8) | cpu.C;
        BC++;
        cpu.B = (BC >> 8) & 0xFF;
        cpu.C = BC & 0xFF;
	printDebug("INC BC", "");
        break;
      }

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
