#include <shared/instructions.h>

namespace ligma {

const size_t MAX_INSTRUCTION_COUNT = 100;

struct InstructionInfo {
    const char* short_name;
    const char* long_name;
    offset_t length;
};

InstructionInfo all_known_instructions [MAX_INSTRUCTION_COUNT] = {
    {"NOOP", "No Operation", 1},
    {"PUSHWORD", "Push Word", 3},
    {"EXECWORD", "Execute Word", 1},
    {"PUSHFLOAT", "Push Float", 3},
    {"PUSHINT", "Push Int", 3},
    {"PUSHSTRING", "Push String", 3},
    {"JMP", "Jump", 3},
    {"JMPIFNOT", "Jump If Not", 3},
    {"PUSHLOCAL", "", 3},
    {"PUSHLAMBDA", "", 3},
    {"RETURN", "", 1},
    {"MAKELIST", "", 3},
    {"MAKEVECTOR", "", 3},
    {"ALLOCLOCAL", "", 3},
    {"DEALLOCLOCAL", "", 3},
    {"REPEAT", "", 3},
    {"FILE", "", 3},
    {"LINE", "", 3},
    {"PRINTSTACK", "", 1},
    {"PRINTINFO", "", 1},
    {"PRINT", "", 1},
    {"LISTNEXT", "", 1},
    {"LISTDATA", "", 1},
    {"LISTNEXTSET", "", 1},
    {"LISTDATASET", "", 1},
    {"VECTOREXTRACT", "", 1},
    {"VECTORINSERT", "", 1},
    {"DROP", "", 1},
    {"OVER", "", 1},
    {"SWAP", "", 1},
    {"DUPLICATE", "", 1},
    {"ROTATE", "", 1},
    {"EQUAL", "", 1},
    {"NOTEQUAL", "", 1},
    {"GREATERTHAN", "", 1},
    {"LESSERTHAN", "", 1},
    {"GREATEROREQUAL", "", 1},
    {"LESSEROREQUAL", "", 1},
    {"AND", "", 1},
    {"OR", "", 1},
    {"NOT", "", 1},
    {"ADD", "", 1},
    {"SUB", "", 1},
    {"MUL", "", 1},
    {"DIV", "", 1},
    {"MOD", "", 1},
    {"FLOOR", "", 1},
    {"ASSIGN", "", 1},
    {"LENGTH", "", 1},
    {"IDENTITY", "", 1},
    {"TYPE", "", 1},
    {"CARRIAGERETURN", "", 1},
    {"EXPLODE", "", 1},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0},
    {NULL, NULL, 0}
};


/// Finds the short name of an instruction.
/// Returns a pointer to string literal if instruction is valid,
/// else returns a null pointer.
const char* BytecodeInstructionShortName (instr_t instruction) {
    if (instruction >= MAX_INSTRUCTION_COUNT) {
        return NULL;
    } else {
        return all_known_instructions[instruction].short_name;
    }
}

/// Finds the long name of an instruction.
/// Returns a pointer to string literal if instruction is valid,
/// else returns a null pointer.
const char* BytecodeInstructionLongName (instr_t instruction) {
    if (instruction >= MAX_INSTRUCTION_COUNT) {
        return NULL;
    } else {
        return all_known_instructions[instruction].long_name;
    }
}

/// Finds the length of an instruction.
/// Returns the length of the instruction, including parameters,
/// if the instruction is valid, else returns zero.
offset_t BytecodeInstructionLength(instr_t instruction) {
    if (instruction >= MAX_INSTRUCTION_COUNT) {
        return 0;
    } else {
        return all_known_instructions[instruction].length;
    }
}

}