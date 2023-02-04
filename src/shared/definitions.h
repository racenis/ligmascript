#ifndef LIGMA_SHARED_DEFINITIONS_H
#define LIGMA_SHARED_DEFINITIONS_H

#include <stdint.h>
#include <stddef.h>

namespace ligma {

/// Bytecode instruction
typedef uint8_t instr_t;

/// Parameter of a bytecode instruction
typedef uint16_t param_t;

/// Offset into memory, in bytes
typedef uint32_t offset_t;

/// Int value type
typedef int32_t int_t;

/// Float value type
typedef float float_t;

/// String value type
typedef char char_t;

/// Atom enum type
typedef uint32_t atom_t;

}

#endif