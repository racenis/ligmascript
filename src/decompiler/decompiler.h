#ifndef LIGMA_DECOMPILER_DECOMPILER_H
#define LIGMA_DECOMPILER_DECOMPILER_H

#include <shared/bytecode.h>

namespace ligma {

class Decompiler {
public:
    Decompiler (Bytecode* bytecode);
    void decompile ();
private:
    Bytecode* bytecode;
};

}

#endif