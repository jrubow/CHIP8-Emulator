#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "instructions.h"
#define SDL_MAIN_HANDLED
#include "SDL2/include/SDL2/SDL.h"


// This program was built with reference to Cowgods Chip-8 technical reference sheet. Located here:
// https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod%27s%20CHIP-8%20Technical%20Reference.pdf

#define MAX_RAM 4096
#define MAX_GENERAL_REGISTERS 16
#define PC_OFFSET 0
#define SP_OFFSET 2
#define MAX_STACK 32
#define MAX_BUFFER 256
#define INSTRUCTION_LENGTH 2
#define F 15
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

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
short I; // 2 byte (16 bit) register
char delayTimer; // 1 byte (8 bit) register
char soundTimer; // 1 byte (8 bit) register
short programCounter; // 2 byte (16 bit) register
char stackPointer = 0; // 1 byte (8 bit) register

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

bool display[SCREEN_HEIGHT][SCREEN_WIDTH];

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

int getAddr(unsigned char *instruction) {
  return (int) ((*instruction) & 0x0FFF);
}

int getx(unsigned char *instruction) {
  return (int) (((*instruction) & 0x0F00) >> 8);
}

int gety(unsigned char *instruction) {
  return (int) (((*instruction) & 0x00F0) >> 4);
}

int getkk(unsigned char *instruction) {
  return (int) ((*instruction) & 0x0FF);
}

int getn(unsigned char *instruction) {
  return (int) ((*instruction) & 0x000F);
}

void loadSprite(short spriteAddr, int n, char Vx, char Vy) {
  genRegs[F] = 0;
  for (int i = 0; i < n; i++) {
    unsigned char sb = memory[spriteAddr + i];
    for (int j = 0; j < SCREEN_WIDTH; j++) {
      int sp = (sb >> (7 - j)) & 1;
      int x = (Vx + j) % SCREEN_WIDTH;
      int y = (Vy + i) % SCREEN_HEIGHT;

      if (sp == 1) {
        if (display[y][x] == 1) {
          genRegs[F] = 1;
        }

        display[y][x] = !display[y][x];
      }
    }
  }
}

void renderDisplay(SDL_Renderer *renderer) {
  const int PIXEL_SIZE = 10;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_Rect pixelRect;
  pixelRect.w = PIXEL_SIZE;
  pixelRect.h = PIXEL_SIZE;

  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (display[y][x]) {
        pixelRect.x = x * PIXEL_SIZE;
        pixelRect.y = y * PIXEL_SIZE;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pixelRect);
      }
    }
  }
  SDL_RenderPresent(renderer);
}

int validateInstruction(unsigned char *instruction) {
  return 0;
}

int executeInstruction();

// Takes in read instruction > verifies the instruction is valid > executes the instruction 
int handleInstruction(unsigned char *instruction) {
  if (validateInstruction(instruction)) {
    printf("%s is an invalid instruction: %d\n", instruction, strerror(errno));
    return 1;
  }
  
  // 0x12340000 >> 28
  int instructionCode = (int) ((*instruction) >> 12);
  int logicalID;
  int skpID;
  int spldID;

  switch (instructionCode) {
    case SYSTEM:
      break;
    case JMP:
      programCounter = getAddr(instruction);
      break;
    case CALL:
      stackPointer++;
      stack[stackPointer] = programCounter;
      programCounter = getAddr(instruction);
      break;
    case SE:
      if (genRegs[getx(instruction)] == getkk(instruction)) {
        programCounter += 2;
      }
      break;
    case SNE:
      if (genRegs[getx(instruction)] != getkk(instruction)) {
        programCounter += 2;
      }
      break;
    case SER:
      if (genRegs[getx(instruction)] == genRegs[gety(instruction)]) {
        programCounter += 2;
      }
      break;
    case LOAD:
      genRegs[getx(instruction)] = getkk(instruction);
      break;
    case ADD:
      genRegs[getx(instruction)] += getkk(instruction);
      break;
    case LOGICAL:
      logicalID = (*instruction) & 0xF;
      switch (logicalID) {
        case 0:
          // LD Vx, Vy
          genRegs[getx(instruction)] = genRegs[gety(instruction)];
          break;
        case 1:
          // OR Vx, Vy
          genRegs[getx(instruction)] |= genRegs[gety(instruction)];
          break;
        case 2:
          // AND Vx, Vy 
          genRegs[getx(instruction)] &= genRegs[gety(instruction)];
          break;
        case 3:
          // XOR Vx, Vy
          genRegs[getx(instruction)] ^= genRegs[gety(instruction)];
          break;
        case 4:
          // ADD Vx, Vy
          genRegs[getx(instruction)] += genRegs[gety(instruction)];
          break;
        case 5:
          // SUB Vx, Vy
          genRegs[getx(instruction)] -= genRegs[gety(instruction)];
          break;
        case 6:
          // SHR Vx {, Vy}
          if (genRegs[getx(instruction)] & 0x1) {
            genRegs[F] = 1;
          } else {
            genRegs[F] = 0;
          }

          genRegs[getx(instruction)] = genRegs[getx(instruction)] >> 1;
          break;
        case 7:
          // SUBN Vx, Vy
          if (genRegs[gety(instruction)] > genRegs[getx(instruction)]) {
            genRegs[F] = 1; // NOT borrow
          } else {
            genRegs[F] = 0;
          }

          genRegs[getx(instruction)] = genRegs[gety(instruction)] - genRegs[getx(instruction)];
          break;
        case 8:
          // SHL Vx {, Vy}
          if (genRegs[getx(instruction)] & 0b10000000) {
            genRegs[F] = 1;
          } else {
            genRegs[F] = 0;
          }

          genRegs[getx(instruction)] = genRegs[getx(instruction)] << 1;
          break;
        default:
          printf("Unrecognized Instruction %s in File: %s\n", instruction, strerror(errno));
          return 1;         
      }
      break;
    case SKIPR:
      if (genRegs[getx(instruction)] != genRegs[gety(instruction)]) {
        programCounter += 2;
      }
      break;
    case LOADI:
      I = getAddr(instruction);
      break;
    case JMPB:
      programCounter = genRegs[0] + getAddr(instruction);
      break;
    case RND:
      genRegs[getx(instruction)] &= (rand() % 256);
      break;
    case DRAW:
      loadSprite(I, getn(instruction), getx(instruction), gety(instruction));
      break;
    case SKIP:
      skpID = (*instruction) & 0xFF;
      switch (skpID) {
        case 0x9E:
          // SKIP Vx
          break;
        case 0xA1:
          // SKNP Vx
          break;
        default:
          printf("Unrecognized Intruction %s in File: %s\n", instruction, strerror(errno));
          return 1;
      }
      break;
    case SPECIAL_LOAD:
      spldID = (*instruction) & 0xFF;
      switch (spldID) {
        case 0x07:
          // LOAD Vx, DT
          genRegs[getx(instruction)] = delayTimer;
          break;
        case 0x0A:
          // LOAD Vx, K
          // TOD....O
          break;
        case 0x15:
          // LOAD DT, Vx
          delayTimer = genRegs[getx(instruction)];
          break;
        case 0x18:
          // LOAD ST, Vx
          soundTimer = genRegs[getx(instruction)];
          break;
        case 0x1E:
          // ADD I, Vx
          I += genRegs[getx(instruction)];
          break;
        case 0x29:
          // TOD...O
          break;
        case 0x33:
          // LOAD B, Vx
          memory[I] = genRegs[getx(instruction)] / 100;
          memory[I + 1] = (genRegs[getx(instruction)] / 10) % 10;
          memory[I + 2] = genRegs[getx(instruction)] % 10;
          break;
        case 0x55:
          // LOAD [I], Vx
          for (int i = 0; i <= getx(instruction); i++) {
            memory[I + i] = genRegs[i];
          }
          break;
        case 0x65:
          // LOAD Vx, [I]
          for (int i = 0; i <= getx(instruction); i++) {
            genRegs[i] = memory[I + i];
          }
          break;
        default:
          printf("Unrecognized Instruction %s in File: %s\n", instruction, strerror(errno));
          return 1;
      }
      break;
    default:
      printf("Unrecognized Instruction %s in File: %s\n", instruction, strerror(errno));
      return 1;
  }


  return 0;  
}


int main(int argc, char *argv[]) {
  // Check that one argument was supplied
  if (argc != 2) {
    printf("Incorrect number of arguments supplied (%d supplied): %s\n", argc, strerror(errno));
    return 1;
  }

  char instruction[INSTRUCTION_LENGTH];

  // Open file
  //FILE *fp = fopen(argv[1], "rb");
  FILE *fp = fopen("./tests/test_opcode.ch8", "rb");
  if (fp == NULL) {
    printf("Cannot open given file: %s\n", strerror(errno));
    return 1;
  }

  // Start SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Cannot start SDL: %s\n", SDL_GetError());
    return 1;
  }

  // Initialize SDL
  SDL_Window *window = SDL_CreateWindow(
      "Chip-8 Emulator",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      640, 320,
      SDL_WINDOW_SHOWN);

  if (window == NULL) {
    printf("Cannot display SDL window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Cannot create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  rewind(fp);
  fread(&memory[0x200], size, 1, fp);
  fclose(fp);
  bool running = true;

  // Read in instructions
  while (running) {
    unsigned short instruction = (memory[programCounter] << 8) | memory[programCounter + 1];
    handleInstruction((unsigned char *)&instruction);
    programCounter += 2;
    
    renderDisplay(renderer);

    // Approximate 60hz
    SDL_Delay(16);
    
    // Update delay timer
    if (delayTimer > 0) {
      delayTimer--;
    }

    // Update sound timer
    if (soundTimer > 0) {
      soundTimer--;
    }
    
  }

  SDL_DestroyWindow(window);

  SDL_Quit();



  return 0;
}


