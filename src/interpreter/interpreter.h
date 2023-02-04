#ifndef INTERPRETER_RUNTIME_H
#define INTERPRETER_RUNTIME_H

#include <shared/definitions.h>
#include <interpreter/word.h>

namespace ligma {
    
void InitInterpreter();
offset_t SubmitBytecode(Bytecode* bytecode);

param_t MakeWord();
param_t MakeWord(const char* symbol);
Word* LookupWord(param_t id);
const char* LookupAtom(atom_t atom);
void CleanWord (Word* word);

}

#endif