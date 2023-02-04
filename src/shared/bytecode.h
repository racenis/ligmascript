#ifndef LIGMA_SHARED_BYTECODE_H
#define LIGMA_SHARED_BYTECODE_H

#include <shared/definitions.h>
#include <shared/allocator.h>

namespace ligma {
class Compiler;
class Decompiler;

/// Contains bytecode and linking information.
    
class Bytecode {
public:
    Bytecode () : bytecode (100), literals (100), symbols (100) {}
    void append(Bytecode*);
protected:
    Allocator bytecode;
    Allocator literals;
    Allocator symbols;
    
    friend class Compiler;
    friend class Decompiler;
    friend offset_t SubmitBytecode (Bytecode*);
};
}

#endif