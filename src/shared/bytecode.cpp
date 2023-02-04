#include <shared/bytecode.h>
#include <shared/instructions.h>

#include <stdio.h>

namespace ligma {

/// Appends a Bytecode.
/// Concatenates the other Bytecode's bytecode and literals,
/// also appends links, but fixes their offsets first.
    
void Bytecode::append(Bytecode* other) {
    // offsets of where the newly appended sections start
    offset_t bytecode_end = this->bytecode.size();
    offset_t literals_end = this->literals.size();
    offset_t symbols_end =  this->symbols.size();
    
    // appending the new sections
    this->bytecode.append(other->bytecode.begin(), other->bytecode.size());
    this->literals.append(other->literals.begin(), other->literals.size());
    this->symbols.append(other->symbols.begin(), other->symbols.size());
    
    // now we need to fix up the offsets in the newly appended sections
    
    char* new_bytecode = this->bytecode.begin() + bytecode_end;
    char* new_bytecode_end = new_bytecode + other->bytecode.size();
    
    while (new_bytecode < new_bytecode_end) {
        instr_t* instruction = (instr_t*) new_bytecode;
        param_t* parameter =   (param_t*) (new_bytecode + sizeof(instr_t));
        
        switch (*instruction) {
            case OPERATION_PUSH_FLOAT:
            case OPERATION_PUSH_INT:
            case OPERATION_PUSH_STRING:
                *parameter += literals_end;
                break;
            case OPERATION_PUSH_LAMBDA:
            case OPERATION_JUMP:
            case OPERATION_JUMP_IF_NOT:
                *parameter += bytecode_end;
                break;
            default:
                break;
        }
        
        new_bytecode += BytecodeInstructionLength(*instruction);
    }
    
    offset_t* new_symbols = (offset_t*) (this->symbols.begin() + symbols_end);
    offset_t* new_symbols_end = (offset_t*) this->symbols.end();
    
    //printf("AAA %d %d %d\n", this->symbols.begin(), new_symbols, new_symbols_end);
    
    while (new_symbols < new_symbols_end) {
        offset_t* symbol_name = new_symbols;
        offset_t* symbol_offset = new_symbols + 1;
        
        //printf("SYMBOL!! %d\n", *symbol_name);
        
        *symbol_name += literals_end;
        *symbol_offset += bytecode_end;
        
        new_symbols += 2;
    }
    
    /*
    int* link = (int*) other->links.begin();
    int* link_end = (int*) other->links.end();
    
    while (link < link_end) {
        int link_type = *link++;
        int link_value = *link++;
        int link_offset = *link++;
        
        links.append(link_type);
        
        switch (link_type) {
            case LINK_WORD:
                links.append(link_value + literals_end);
                links.append(link_offset + bytecode_end);
                break;
            case LINK_LITERAL:
                links.append(link_value + literals_end);
                links.append(link_offset + bytecode_end);
                break;
            case LINK_OFFSET:
                links.append(link_value + bytecode_end);
                links.append(link_offset + bytecode_end);
                break;
        }
    }*/
}

}