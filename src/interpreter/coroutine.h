#ifndef INTERPRETER_COROUTINE_H
#define INTERPRETER_COROUTINE_H

#include <shared/definitions.h>
#include <interpreter/stacks.h>

namespace ligma {
    
class Coroutine {
public:
    Coroutine (offset_t program = 0);
    void run (size_t iterations);
    void reset (offset_t program);
    bool is_finished ();
    const char* get_file_name();
    offset_t get_line_number();
private:
    RefStack ref_stack;
    JmpStack jmp_stack;
    LclStack lcl_stack;
    
    offset_t program_counter;
    offset_t file_counter;
    offset_t line_counter;
};

}

#endif