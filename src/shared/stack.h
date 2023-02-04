#ifndef LIGMA_SHARED_STACK_H
#define LIGMA_SHARED_STACK_H

#include <shared/definitions.h>

namespace ligma {

/// Allocates memory for a stack.
/// For use in stack implementations only.
void* StackAllocate(size_t size);

/// Deallocates memory for a stack.
/// For use in stack implementations only.
void StackDeallocate(void* memory);
    
/// Stack template.
template <typename T>
class Stack {
public:
    Stack (size_t initial_size) {
        stack_start = (T*) StackAllocate(initial_size * sizeof(T));
        stack_end = stack_start + initial_size;;
        stack_top = stack_start;
    }
    
    ~Stack() {
        StackDeallocate(stack_start);
        stack_start = NULL;
        stack_end = NULL;
        stack_top = NULL;
    }
    
    void push (T value) {
        *++stack_top = value;
    }
    
    T pop () {
        return *stack_top--;
    }
    
    T top () {
        return *stack_top;
    }
    
    bool is_empty () {
        return stack_start == stack_top;
    }
    
    bool is_full () {
        return !(stack_top < stack_end);
    }
    
private:
    T* stack_start;
    T* stack_end;
    T* stack_top;
};

}
#endif