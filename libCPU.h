#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Bit Shifiting, Byte Masks, Extensions */
#define MSB32MASK 	0x80000000	// Mask for MSB if 32-bits
#define MSBTOLSB  	31			// Shift bits from MSB to LSB
#define BYTE_MASK 	0xFF 
#define SHIFT_BYTE 	8			// for shifting by a byte
#define CYCLES    	4

/* Sizes */
#define MEMSIZE 	  16000 			  // Max memory size
#define MAX32	  	  0xFFFFFFFF		// Value used to help check carryFlag
#define REG_SIZE  	0x4				    // to move to the next inst.
#define BYTE_SIZE 	0x1
#define RF_SIZE   	0x10 			    // Register size = 16-bits
#define THUMB_SIZE  0x2

#define LINE_BREAK  0x6

/* Special Registers in Register File Offsets */
#define RF_SP   0xD
#define RF_LR   0xE
#define RF_PC   0xF
#define SP      regfile[RF_SP]		// Stack Pointer 
#define LR      regfile[RF_LR]		// Link Register
#define PC      regfile[RF_PC]		// Program Counter

/* Instruction Registers */
#define IR0 (unsigned)ir >> 16 		// set ir0 to the lower 16 bits of the 32-bit inst.
#define IR1 ir & 0xFFFF 			// set ir1 to the upper 16 bits of the 32-bit inst.

/* Instruction Formats */
#define FORMAT		  (unsigned)cir >> 13	// shift until the last 3-MSbits EX: 110
#define LOAD_STORE 	FORMAT == 0x1 		  // to check for Load or Store inst.
#define STOP 	  	  cir == 0xE000		    // to check for stop inst.
#define COND_BRANCH FORMAT == 0x4		    // to check for COND_BRANCH inst.
#define BRANCH      FORMAT == 0x6		    // to check for BRANCH inst.
#define DATA_PROC   FORMAT == 0x0 		 // to check for Data Processing inst.

/* Data Processing OpCodes */
#define DATA_SUB 0x2 == OPERATION 		// to check for SUB inst.
#define DATA_ADD 0x4 == OPERATION 		// to check for ADD inst.

/* Instruction Fields */
#define RN			    ((cir >> 4) & 0xF)		// to get the reg 1 number from the inst.
#define RD			    cir & 0xF 				    // to get the reg 2 number from the inst.
#define BYTE_BIT	  ((cir >> 10) & 0x1)		// to check if should store/load single byte 
#define LOAD_BIT	  ((cir >> 11) & 0x1)		// to check for load inst.
#define CONDITION   ((cir >> 8) & 0xF)		// to check which code is selected for the COND_BRANCH
#define LINK_BIT    ((cir >> 12) & 0x1)		// to check the 13th bit of the cir if set  
#define OFFSET12    cir & 0xFFF 			    // to get the offset location 
#define COND_ADDR   cir & 0xFF 				    // to get the offset location for COND_BRANCH
#define OPERATION   ((cir >> 8) & 0xF)		// to check if SUB or ADD selected from the inst.


/* Branch Condition Codes Suffixes 
*  Each one checks which code is 
*  selected from the inst.
*/
#define EQ 0x0 == CONDITION
#define NE 0x1 == CONDITION
#define CS 0x2 == CONDITION
#define CC 0x3 == CONDITION
#define MI 0x4 == CONDITION 
#define PL 0x5 == CONDITION
#define HI 0x8 == CONDITION
#define LS 0x9 == CONDITION
#define AL 0xE == CONDITION

/* Flags */
int signFlag;
int zeroFlag;
int stopFlag;
int flag_ir;
int flag_carry;

/* Registers */
int cir;	// Unoffical register for current instructin
int mar;	// Memory Address Register 
int mbr;	// Memory Buffer Register 
int regfile[RF_SIZE];	// Registers  Array
int ir;
int alu;	// Arithmetic logic unit 
