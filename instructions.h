// CHIP-8 Instruction Set
//

// CLS | clears the display
#define CLS 0x00E0

// RET | return from a subroutine
#define RET 0x00EE

// JMP addr | jumps to addr | [100, 200)
# define JMP 0x100

// CALL addr | calls a subroutine | [200, 300)
# define CALL 0x200

// SE Vx, kk | skips to the next instruction if Vx = kk
