#include <stdio.h>
#include <stdlib.h>

#include "main.h"

// #define DEBUG


Optype getOpcodeType(const Opcode opcode) {
  switch (opcode) {
    case OPCODE_LOAD:
    case OPCODE_STORE:
      return OPTYPE_MEM;
    case OPCODE_IN:
      return OPTYPE_IN;
    default:
      return OPTYPE_DATA_PATH;
  }
}


StatusCode executeInstruction(VM *vm, Instruction *instruction) {
  const int8_t opcode = instruction->opcode_mask.opcode;
#ifdef DEBUG
  printf("Opcode: %i\n", opcode);
#endif

  if (opcode == OPCODE_SWAP) {  // =====> mem
    vm->regs[instruction->dpath.reg1] += vm->regs[instruction->dpath.reg2];
    vm->regs[instruction->dpath.reg2] = vm->regs[instruction->dpath.reg1]
                                        - vm->regs[instruction->dpath.reg2];
    vm->regs[instruction->dpath.reg1] -= vm->regs[instruction->dpath.reg2];
  } else if (opcode == OPCODE_LOAD) {
  } else if (opcode == OPCODE_STORE) {
  } else if (opcode == OPCODE_IN) {  // =====> in
#ifdef DEBUG
      printf("[DEBUG]: Store value %i to register %i\n", instruction->mem.arg, instruction->mem.reg);
#endif
    vm->regs[instruction->in.reg] = instruction->in.value;
  } else if (opcode == OPCODE_OUT) {  // =====> dpath
#ifdef DEBUG
      printf("[DEBUG]: Print register %i\n", instruction->dpath.reg1);
#endif
    printf("%i", vm->regs[instruction->dpath.reg1]);
  } else if (opcode == OPCODE_IADD) {
    vm->regs[instruction->dpath.reg1] += vm->regs[instruction->dpath.reg2];
  } else if (opcode == OPCODE_IMUL) {
    vm->regs[instruction->dpath.reg1] *= vm->regs[instruction->dpath.reg2];
  } else if (opcode == OPCODE_IDIV) {
    vm->regs[instruction->dpath.reg1] /= vm->regs[instruction->dpath.reg2];
  }

  return OK;
}


void executeBytecodeFile(FILE *f) {
  VM *vm = malloc(sizeof(Instruction));
  if (vm == NULL) {
    exit(-1);
  }
  Instruction instruction;
  while (1) {
    const int is_not_eof = fread((char*)&instruction, 1, sizeof(Instruction), f);
    if (!is_not_eof) break;
#ifdef DEBUG
    const enum StatusCode status = executeInstruction(vm, &instruction);
    printf("[DUBUG]: Status code: %i\n", status);
#else
    executeInstruction(vm, &instruction);
#endif
  }
  free(vm);
}


int8_t parseInt8Word(const char *word) {
  return atoi(word);
}


Opcode parseOpcodeWord(const char *word) {
  const char prefixes[11][2] = {
    "lo",
    "st",
    "in",
    "ou",
    "sw",
    "ia",
    "im",
    "is",
    "id",
    "la",
    "ju",
  };
  for (int i = 0; i < 11; i++) {
    if (word[0] == prefixes[i][0] && word[1] == prefixes[i][1]) {
      return i+1;
    }
  }
}


int16_t parseInt16Word(const char *word) {
  printf("Parsed int16 word: %s -> %i\n", word, atoi(word));
  return atoi(word);
}


/**
 * Constructs instruction from provided words
 */
Instruction parseInstructionWords(char words[][40]) {
  const Opcode opcode = parseOpcodeWord(words[0]);
  const Optype optype = getOpcodeType(opcode);

  if (optype == OPTYPE_IN) {
    return (Instruction){.in = {.opcode = opcode, .reg = parseInt8Word(words[1]), .value = parseInt16Word(words[2])}};
  }
  if (optype == OPTYPE_MEM) {
    return (Instruction){.mem = {.opcode = opcode, .reg = parseInt8Word(words[1]), .arg = parseInt16Word(words[2])}};
  }
  if (optype == OPTYPE_DATA_PATH) {
    return (Instruction){.dpath = {
      .opcode = opcode,
      .reg1 = parseInt8Word(words[1]),
      .reg2 = parseInt8Word(words[2]),
      .reg3 = parseInt8Word(words[3]),
    }};
  }
  if (optype == OPTYPE_CONTROL_FLOW) {
    exit(-1);
  }
  exit(-1);
}


/**
* Compiles text from provided descriptor into bytecode and writes bytecode to another descriptor
*/
void compileSourceFile(FILE *source_f, FILE *destination_f) {
  char currentChar;
  char wordPosition = 0;
  char instructionPosition = 0;
  char currentWords[4][40];
  char symbolN = 0;
  while (1) {
    printf("Read symbol %i\n", symbolN++);
    const int is_not_eof = fread(&currentChar, sizeof(char), sizeof(char), source_f);
    if (!is_not_eof) break;
    printf("Char: `%c` (%i)\n", currentChar, currentChar);

    printf("instructionPosition: %i, wordPosition: %i\n", instructionPosition, wordPosition);
    if (currentChar == ' ') {
      currentWords[instructionPosition][wordPosition] = '\0';
      wordPosition = 0;
      printf("Current word (finished): `%s`\n", currentWords[instructionPosition]);
      instructionPosition++;
      continue;
    }
    if (currentChar == '\n') {
      currentWords[instructionPosition][wordPosition] = '\0';
      wordPosition = 0;
      printf("Current word (finished): `%s`\n", currentWords[instructionPosition]);
      instructionPosition = 0;
      printf("%p\n", currentWords);
      Instruction instruction = parseInstructionWords(currentWords);
      printf("Write instruction\n");
      fwrite((char*)&instruction, 1, sizeof(Instruction), destination_f);
      continue;
    }
    currentWords[instructionPosition][wordPosition++] = currentChar;
  }
}


int main() {
  FILE *source_file = fopen("program-example.simple", "rb");
  FILE *destination_file = fopen("program-example.simplec", "wb");
  if (source_file == NULL || destination_file == NULL) {
    exit(-1);
  }
  compileSourceFile(source_file, destination_file);
  fclose(source_file);
  fclose(destination_file);

  source_file = fopen("program.samplec", "rb");
  if (source_file == NULL) {
    exit(-1);
  }
  printf("Start execution...\n");
  executeBytecodeFile(source_file);
}
