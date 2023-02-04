#include <decompiler/decompiler.h>
#include <shared/instructions.h>
#include <stdio.h>

namespace ligma {
    
Decompiler::Decompiler (Bytecode* bytecode) {
    this->bytecode = bytecode;
}

void Decompiler::decompile () {
    instr_t* first = (instr_t*) this->bytecode->bytecode.begin();
    instr_t* last = (instr_t*) this->bytecode->bytecode.end();
    
    printf("===== OPCODES =====\n");
    
    instr_t* it = first;
    while (it < last) {
        offset_t offset = it - first;
        size_t length = BytecodeInstructionLength(*it);
        const char* name = BytecodeInstructionShortName(*it);
        name = name ? name : "INVALID";
        
        printf("%03d %s", offset, name);
        if (length == 3) printf(" %d", *((param_t*)(it+1)));
        printf("\n");
        
        // move the iterator by instruction length
        length = length ? length : 1;
        it += length;
    }
    
    printf("===== SYMBOLS =====\n");
    
    offset_t* symbol = (offset_t*) this->bytecode->symbols.begin();
    offset_t* last_symbol = (offset_t*) this->bytecode->symbols.end();
    
    while (symbol < last_symbol) {
        offset_t* symbol_name = symbol;
        offset_t* symbol_offset = symbol + 1;
        
        char* symbol_name_str = this->bytecode->literals.begin() + *symbol_name;
        
        printf("%03d %s\n", *symbol_offset, symbol_name_str);
        
        symbol += 2;
    }
    
    printf("=====   END   =====\n");
}

}