#pragma once

#include <stdlib.h>
#include <stdio.h>


typedef enum {
  OPTYPE_MEM          = 0x1,
  OPTYPE_IN           = 0x2,
  OPTYPE_DATA_PATH    = 0x3,
  OPTYPE_CONTROL_FLOW = 0x4,
} Optype;


enum Opcode {
  // =====> mem struct instructions
  OPCODE_LOAD      = 0x1,
  OPCODE_STORE     = 0x2,
  // ======> in struct instructions
  OPCODE_IN        = 0x3,
  // =====> data path instructions
  // output value of reg1 to stdin, using reg2 output mode
  OPCODE_OUT       = 0x4,
  // swap values of reg1 and reg2
  OPCODE_SWAP      = 0x5,
  // add register 1 to register 2 and store result within register 1 (i - inpace. inspired by Python's `__iadd__`)
  OPCODE_IADD      = 0x6,
  // multiplie register 1 on register 2 and store result within register 1 (i - inpace)
  OPCODE_IMUL      = 0x7,
  // sub register 1 with register 2 and store result within register 1 (i - inpace)
  OPCODE_ISUB      = 0x8,
  // divide register 1 on register 2 and store result within register 1 (i - inpace)
  OPCODE_IDIV      = 0x9,
};


typedef int8_t Opcode;


Optype getOpcodeType(Opcode opcode);


struct InstructionOpcodeMask {
  Opcode opcode;
};


/**
 * For mem manipulation instructions
 */
struct InstructionMem {
  Opcode opcode;
  int8_t reg;
  int16_t arg;
};


/**
 * For instruction that inputs 16 bit value into register
 */
struct InstructionIn {
  Opcode opcode;
  int8_t reg;
  int16_t value;
};


/**
 * For instructions that requires data path based manipulation with values, stored within registers.  Takes 3 registers,
 *  refer to opcode to meaning of each.
 */
struct InstructionDataPath {
  Opcode opcode;
  int8_t reg1;
  int8_t reg2;
  int8_t reg3;
};


typedef union {
  struct InstructionOpcodeMask opcode_mask;
  struct InstructionMem mem;
  struct InstructionIn in;
  struct InstructionDataPath dpath;
} Instruction;

_Static_assert (sizeof(Instruction) == 32/8);


typedef enum {
  OK,
  ERR,
} StatusCode;


typedef struct {
  int32_t regs[10];
  int32_t program_counter;
} VM;


StatusCode executeInstruction(VM *vm, Instruction *instruction);


void executeBytecodeFile(FILE *f);


int8_t parseInt8Word(const char *word);


Opcode parseOpcodeWord(const char *word);


int16_t parseInt16Word(const char *word);


/**
 * Constructs instruction from provided words
 */
Instruction parseInstructionWords(char words[][40]);


/**
* Compiles text from provided descriptor into bytecode and writes bytecode to another descriptor
*/
void compileSourceFile(FILE *source_f, FILE *destination_f);
