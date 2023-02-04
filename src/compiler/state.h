#ifndef COMPILER_STACK_H
#define COMPILER_STACK_H

#include <shared/definitions.h>
#include <shared/stack.h>

// TODO: replace stack template with a custom stack

namespace ligma {

enum compiler_state_t {
    COMPILE_GLOBAL,         // default state
    COMPILE_PUSH_WORD,      // between []
    COMPILE_BLOCK,          // when a block has just been opened
    COMPILE_IF,
    COMPILE_ELSE,
    COMPILE_LAMBDA,         // compiling a lambda
    COMPILE_LIST,           // a list
    COMPILE_VECTOR,         // a vecotr
    COMPILE_DECLARATION     // declaring locals
};

struct CompilerStack;

class CompilerState {
public:
    CompilerState();
    ~CompilerState();
    void push (compiler_state_t state, offset_t offset);
    void pop ();
    void increment();
    compiler_state_t get_state();
    offset_t get_offset();
    size_t get_count();
    void add_local(offset_t variable);
    param_t find_local(offset_t variable);
    param_t get_local_count();
private:
    CompilerStack* stack;
};
    
}

#endif