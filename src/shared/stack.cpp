#include <shared/stack.h>
#include <stdlib.h>

namespace ligma {

void* StackAllocate(size_t size) {
    return malloc(size);
}

void StackDeallocate(void* memory) {
    free(memory);
}
    
}