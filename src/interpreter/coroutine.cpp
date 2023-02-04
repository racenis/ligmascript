#include <interpreter/coroutine.h>

namespace ligma {
Coroutine::Coroutine (offset_t program) {
    this->program_counter = program;
    
    this->file_counter = 0;
    this->line_counter = 0;
}

void Coroutine::reset (offset_t program) {
    this->program_counter = program;
    
    this->file_counter = 0;
    this->line_counter = 0;
    
    while (!this->jmp_stack.is_empty()) this->jmp_stack.pop();
    // TODO: empty the local stack too
}

bool Coroutine::is_finished () {
    return this->jmp_stack.is_empty();
}

// the actual run() method is in the interpreter.cpp
}