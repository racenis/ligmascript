#include <compiler/state.h>
#include <compiler/error.h>
#include <stdlib.h>

namespace ligma {

struct LocalVariable {
    offset_t name;
    LocalVariable* next;
};

struct CompilerStateRecord {
    compiler_state_t state;
    offset_t offset;
    size_t count;
    size_t local_count;
    LocalVariable* locals;
};

struct CompilerStack {
    CompilerStack() {
        begin = (CompilerStateRecord*) calloc (100, sizeof(CompilerStateRecord));
        end = begin + 100;
        top = begin + 1;
        
        begin->state = COMPILE_GLOBAL;
        begin->count = 0;
        begin->offset = 0;
        begin->local_count = 0;
        begin->locals = NULL;
    }
    
    ~CompilerStack() {
        free(begin);
        begin = end = top = NULL;
    }
    
    CompilerStateRecord* begin;     // first element
    CompilerStateRecord* end;       // one past the last element
    CompilerStateRecord* top;       // element on top
};

    
CompilerState::CompilerState() {
    stack = new CompilerStack();
}

CompilerState::~CompilerState() {
    delete stack;
}
    
void CompilerState::push (compiler_state_t state, offset_t offset) {
    if (++stack->top == stack->end) {
        throw CompilerError (COMPILER_ERROR_STATE_OVERFLOW);
    }
    
    stack->top->state = state;
    stack->top->offset = offset;
    stack->top->count = 0;
    stack->top->local_count = 0;
    stack->top->locals = NULL;
}

void CompilerState::pop () {
    for (LocalVariable* it = stack->top->locals; it != NULL;) {
        LocalVariable* next = it->next;
        free (it);
        it = next;
    }
    
    if (--stack->top == stack->end) {
        // if we've popped a value and the stack is empty afterwards, that
        // means that we've popped off the COMPILE_GLOBAL state that should
        // always be on the bottom of the stack and ruined the compiler state
        throw CompilerError (COMPILER_ERROR_STATE_UNDERFLOW);
    }
}

void CompilerState::increment() {
    stack->top->count++;
}

compiler_state_t CompilerState::get_state() {
    return stack->top->state;
}

offset_t CompilerState::get_offset() {
    return stack->top->offset;
}

size_t CompilerState::get_count() {
    return stack->top->count;
}

void CompilerState::add_local(offset_t variable) {
    LocalVariable* new_local = (LocalVariable*) malloc(sizeof(LocalVariable));
    
    new_local->name = variable;
    new_local->next = stack->top->locals;
    
    stack->top->local_count++;
    stack->top->locals = new_local;
}

param_t CompilerState::find_local(offset_t name) {
    param_t depth = 0;
    for (CompilerStateRecord* rec = stack->top; rec->state != COMPILE_GLOBAL; rec--) {
        LocalVariable* ptr = rec->locals;
        param_t counter = 0;
        
        while (ptr != NULL && ptr->name != name) {
            ptr = ptr->next;
            counter++;
        }
        
        if (ptr && ptr->name == name) {
            return depth + counter;
        } else {
            depth += counter;
        }
    }
    
    return -1;
}

param_t CompilerState::get_local_count() {
    return stack->top->local_count;
}


    
}