// CHIP-8 Instruction Set
// Variables used below to represent variable hexadecimal values
// nnn or addr - represents a 12 bit address.
// n (nibble)  - 4 bit value.
// x           - lower 4 bits of the high byte in the instruction.
// y           - upper 4 bits of the low byte of the instruction.
// kk or byte  - 1 byte value (8 bits). Lowest byte of the instrcution.
// Vx/Vy       - refers to one of the general purpose registers.

// 0nnn | SYS addr/nnn | jumps to a machine code at nnn | [0x0F00, 0x1000]
#define SYSTEM 0x0F00

// 00E0 | CLS | clears the display
#define CLS 0x00E0

// 00EE | RET | return from a subroutine
#define RET 0x00EE

// 1nnn | JMP addr | jumps to addr/nnn | [0x1, 0x2)
#define JMP 0x1

// 2nnn | CALL addr | calls a subroutine | [0x2, 0x3)
#define CALL 0x2

// 3xkk | SE Vx, kk | skips to the next instruction if Vx = kk | [0x3, 0x4)
#define SE 0x3000

// 4xkk | SNE Vx, byte | skips to the next instruction if Vx != kk | [0x4, 0x5)
#define SNE 0x4

// 5xy0 | SE Vx, Vy | skips to the next instruction if Vx == Vy | [0x5, 0x6)
#define SER 0x5

// 6xkk | LOAD Vx, byte | loads kk into Vx | [0x6, 0x7)
#define LOAD 0x6

// 7xkk | ADD Vx, byte | adds kk to Vx | [0x7, 0x8)
#define ADD 0x7

// Logical Operators | [0x8000,0x9000]
// 8xy0 | LOAD Vx, Vy | loads Vy into Vx
// 8xy1 | OR Vx, Vy | Vx OR Vy
// 8xy2 | AND Vx, Vy | Vx AND Vy
// 8xy3 | XOR Vx, Vy | Vx AND Vy
// 8xy4 | ADD Vx, Vy | Adds Vy to Vx and sets VF to carry if result is greater
// than 255
// 8xy5 | SUB Vx, Vy | Subtracts Vy from Vx and sets VF to NOT borrow
// 8xy6 | SHR Vx {, Vy} | Sets Vx to Vx SHR 1
// 8xy7 | SUBN Vx, Vy | Sets Vx to (Vy - Vx) - reverse of SUB
// 8xyE | SHL Vx {, Vy} | Vx = Vx SHL 1
#define LOGICAL 0x8

// 9xy0 | SNE Vx, Vy | Skips to the next instruction if Vx != Vy
#define SKIPR 0x9

// Annn | LOAD I, addr | I = nnn
#define LOADI 0xA

// Bnnn | JMP V0, addr | jumps to the location nnn + V0
#define JMPB 0xB

// Cxkk | RND Vx, byte | sets Vx to a random byte AND kk | [0xC, 0xD)
#define RND 0xC

// Dxyn | DRAW Vx, Vy, n | Displays n-byte sprite at memory location I at (Vx,
// Vy) and sets VF to collision | [0xD, 0xE)
#define DRAW 0xD

// Skip  Instructions
// Ex9E | SKIP Vx | skips next instruction if key with value Vx pressed
// ExA1 | SKIPN Vx | skips next instruction if key with value Vx not pressed
#define SKIP 0xE

// Special Load Instructions
// Fx07 | LOAD Vx, DT | loads the delay timer register into Vx
// Fx0A | LOAD Vx, K  | waits for next key press and loads key value into Vx
// Fx15 | LOAD DT, Vx | loads Vx into the delay timer register
// Fx18 | LOAD ST, Vx | loads Vx into the sound timer register
// Fx1E | ADD I, Vx   | Adds Vx to I
// Fx29 | LOAD F, Vx  | Sets I to the location of sprite for digit Vx
// Fx33 | LOAD B, Vx  | Separates Vx into hundreds, tens, ones at I, I+1, I+2
// Fx55 | LOAD (I),Vx | Stores registers V0 to Vx in mem loc starting at I
// Fx65 | LOAD Vx,(I) | Read registers V0 to Vx from mem loc starting at I
#define SPECIAL_LOAD 0xF
