#ifndef LIGMA_SHARED_ALLOCATOR_H
#define LIGMA_SHARED_ALLOCATOR_H

#include <shared/definitions.h>

namespace ligma {

/// Stack based allocator.
/// Stores data on the heap. You can append additional data to the end of
/// the existing data by calling the append() methods. If the initially
/// allocated space is exceeded, then the allocator will automatically
/// reallocate the data. This means that you shouldn't store pointers into
/// the data, but instead should store memory offsets off of the begin().    

class Allocator {
public:
    Allocator(size_t initial_size);
    ~Allocator();

    offset_t append (char value);
    offset_t append (short value);
    offset_t append (unsigned char value);
    offset_t append (unsigned short value);
    offset_t append (float value);
    offset_t append (int value);
    offset_t append (unsigned int value);
    offset_t append (const void* value, size_t size);
    offset_t insert (offset_t offset, unsigned short value);
    offset_t size ();
    void resize (size_t size);
    char* begin ();
    char* end ();

private:
    char* memory_start;
    char* memory_end;
    size_t memory_size;
};

}
#endif