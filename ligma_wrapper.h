#ifndef LIGMA_WRAPPER_H
#define LIGMA_WRAPPER_H

#include <cstdint>

namespace ligma {
    typedef uint8_t instr_t;    // bytecode instruction
    typedef uint16_t param_t;   // bytecode instruction parameter

    /// loads a file from disk, compiles it and interprets the code
    void LoadFile (const char* filename);

    /// returns the id number of a word from its name
    param_t ParseName (const char* name);

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
