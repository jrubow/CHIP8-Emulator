#include <stdlb.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <instructions.h>

// This program was built with reference to Cowgods Chip-8 technical reference sheet. Located here:
// https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod%27s%20CHIP-8%20Technical%20Reference.pdf

#define MAX_RAM 4096
#define MAX_GENERAL_REGISTERS 16
#define REGISTER_I 16
#define MAX_SPECIAL_REGISTERS 2
#define MAX_PSEUDO_REGISTERS 3
#define PC_OFFSET 0
#define SP_OFFSET 2
#define MAX_STACK 32
#define MAX_BUFFER 256
#define INSTRUCTION_LENGTH 2

// ---------------------------------------------------------------------------------------------------------------------
// |    MEMORY                                                                                                         |
// ---------------------------------------------------------------------------------------------------------------------
// Chip-8 uses a total of 4kb of RAM from location 0x000 to 0xFFF. The first 512 bytes are where the original intepreter 
// was stored this sectio of memory is off limits to any programs that are run. Programs are stored at location 0x200 and
// up.

unsigned char memory[MAX_RAM];

// ---------------------------------------------------------------------------------------------------------------------
// |    REGISTERS                                                                                                      |
// ---------------------------------------------------------------------------------------------------------------------
// There are 16 general purpose 1 byte (8 bit) registers, named as Vx. x represents a digit (hexadecimal) between 0 and 
// 15. Additionally there is a 2 byte (16 bit) register called I which is usually used to store memory addresses. VF is   
// used as a flag by certain instructions. There are two special 1 byte registers for delay and sound timers -- when
// they are not zero they are decremented at 60Hz. There are also program non-accessible registers. The program counter
// register is a 2 byte (16 bit) register and stores the current execution address. 

unsigned char genRegs[MAX_GENERAL_REGISTERS];
unsigned char I[REGISTER_I];
unsigned char specialRegs[MAX_SPECIAL_REGISTERS];
unsigned char pseudoRegs[MAX_PSEUDO_REGISTERS];
// pseudo registers offset of:
//     - 0x0 : Program counter
//     - 0x2 : Stack Pointer

// ---------------------------------------------------------------------------------------------------------------------
// |   STACK                                                                                                           |
// ---------------------------------------------------------------------------------------------------------------------
// The stack contains 16, 2 byte values, therefore, there are only 16 allowed levels of nested subroutines.

unsigned char stack[MAX_STACK];

// ---------------------------------------------------------------------------------------------------------------------
// |   DISPLAY                                                                                                         |
// ---------------------------------------------------------------------------------------------------------------------
// The display is 64x32 pixels monochrome. Chip-48 supports 128x64 pixels as a mode. Sprites are 8x15 pixels. There is 
// also a list of built in sprites stored in the intepreter area of memory.

// TODO

// ---------------------------------------------------------------------------------------------------------------------
// |   TIMERS AND SOUND                                                                                                |
// ---------------------------------------------------------------------------------------------------------------------
// There are two different timers. The delay timer is ACTIVE when the delay timer register (DT) is non-zero, it also
// subtracts 1 from the DT register at 60Hz. When the DT register reaches 0, it deactivates.
// The sound timer is active when the sound timer register (ST) is non-zero. It decrements at 60Hz and while it is greater
// than zero the buzzer sounds. When the ST register reaches zero it deactivates.

// Delay Timer Register (DT) : located at specialRegs offset 0x0
// Sound Timer Register (ST) : located at specialRegs offset 0x1

bool delayActive = false;
bool soundActive = false;


// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------

int validateInstruction();
int executeInstruction();

// Takes in read instruction > verifies the instruction is valid > executes the instruction 
int handleInstruction(char *instruction) {
  int instructionCode = (*((int *) instruction)) >> 4;

  switch (instructionCode) {
    case SYSTEM:
      break;
    case JMP:
      break;
    case CALL:
      break;
    case SE:
      break;
    case SNE:
      break;
    case SE:
      break;
    case LOAD:
      break;
    case ADD:
      break;
    case LOADR:
      break;
    case LOGICAL:
      break;
    case SKIPR:
      break;
    case LOADI:
      break;
    case JMPB:
      break;
    case RND:
      break;
    case DRAW:
      break;
    case SKIP:
      break;
    case SPECIAL_LOAD:
      break;
    default:
      printf("Unrecognized Instruction in File: %d\n", stderror(errno));
      return 1;
  }


  return 0;  
}


int main(int argc, char **argv) {
  // Check that one argument was supplied
  if (argc != 1) {
    printf("Incorrect number of arguments supplied (%d supplied): %s\n", argc, stderror(errno));
    return 1;
  }

  char instruction[INSTRUCTION_LENGTH];

  // Open file
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("Cannot open given file: %d\n", stderror(errno));
    return 1;
  }
  
  // Read in instructions
  while (fscanf(fp, "%2s\n", instruction) == 1) {
    handleInstruction(instruction);
  }



  return 0;
}


