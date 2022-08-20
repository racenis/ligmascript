#ifndef LIGMA_WRAPPER_H
#define LIGMA_WRAPPER_H

#include <cstdint>

namespace ligma {
    typedef uint8_t instr_t;    // bytecode instruction
    typedef uint16_t param_t;   // bytecode instruction parameter

    enum WordType {
        STRING,                         // c-style null-terminated string
        FLOAT32, FLOAT64,               // float and double
        INT8, INT16, INT32, INT64,      // signed integers
        UINT8, UINT16, UINT32, UINT64   // unsigned integers
    };

    /// loads a file from disk, compiles it and interprets the code
    void LoadFile (const char* filename);

    /// returns the id number of a word from its name
    param_t ParseName (const char* name);

    /// sets the value of the word
    void SetValue (WordType type, size_t size, bool read_only, param_t word, void* address);

    /// starts an interpreter instance that will execute the word
    void Interpret (param_t word);

    /// runs the interpreter on all of the instances
    void Update ();

    /// starts the REPL loop on standard input
    void StartConsole ();

    /// stops the REPL loop on standard input
    void StopConsole ();

}

#endif //LIGMA_WRAPPER_H
