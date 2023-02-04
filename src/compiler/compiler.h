#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include <shared/definitions.h>
#include <shared/bytecode.h>
#include <compiler/token.h>
#include <compiler/state.h>
#include <string>
#include <map>

// TODO: replace the std::map in here with a custom hashmap

namespace ligma {    

class Compiler {
public:
    Compiler (Bytecode* bytecode);
    ~Compiler ();
    
    void compile (const char* code);
    void compile_token (token_t token_type, const char* token, token_t next_token);
    
    void compile_from_file (const char* filename);
    
    const char* get_file_name();
    offset_t get_line_number();
    
    bool is_done ();
private:
    Bytecode* bytecode;
    CompilerState state;
    std::map<std::string, offset_t> word_map;
    
    char* lexing_string;
    
    offset_t file_counter;
    offset_t line_counter;
    
    offset_t append_instr (instr_t instr);
    offset_t append_param (param_t param);
    offset_t insert_param (offset_t offset, param_t param);
    offset_t append_float (float value);
    offset_t append_int (int value);
    offset_t append_string (const char* str);
    void append_symbol (offset_t name, offset_t offset);
};

}
#endif