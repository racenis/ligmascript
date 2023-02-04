#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <shared/allocator.h>

namespace ligma {
const size_t ALLOCATOR_SIZE_STEP = 100;

/// Initializes the allocator with size of initial_size.
Allocator::Allocator(size_t initial_size) {
    this->memory_start = (char*) malloc(initial_size);
    this->memory_end = memory_start;
    this->memory_size = initial_size;
}

/// Frees the allocated memory.
Allocator::~Allocator() {
    free(this->memory_start);

    this->memory_start = NULL;
    this->memory_end = NULL;
    this->memory_size = 0;
}

/// Appends a char to the end of the data.
offset_t Allocator::append(char value) {
    return append(&value, sizeof(char));
}

/// Appends a short to the end of the data.
offset_t Allocator::append(short value) {
    return append(&value, sizeof(short));
}

/// Appends a unsigned char to the end of the data.
offset_t Allocator::append(unsigned char value) {
    return append(&value, sizeof(unsigned char));
}

/// Appends a unsigned short to the end of the data.
offset_t Allocator::append(unsigned short value) {
    return append(&value, sizeof(unsigned short));
}

/// Appends a float to the end of the data.
offset_t Allocator::append(float value) {
    return append(&value, sizeof(float));
}

/// Appends a int to the end of the data.
offset_t Allocator::append(int value) {
    return append(&value, sizeof(int));
}

/// Appends a unsigned int to the end of the data.
offset_t Allocator::append(unsigned int value) {
    return append(&value, sizeof(unsigned int));
}

/// Insert a unsigned short in the offset specified.
offset_t Allocator::insert (offset_t offset, unsigned short value) {
    *((unsigned short*)(this->memory_start + offset)) = value;
    return offset;
}

/// Appends arbitrary data to the end of the data.
offset_t Allocator::append (const void* value, size_t size) {
    char* new_memory_end = this->memory_end + size;

    if (new_memory_end >= this->memory_start + this->memory_size) {
        this->resize(this->memory_size + ALLOCATOR_SIZE_STEP);
        return this->append(value, size);
    }

    offset_t new_value_offset = this->memory_end - this->memory_start;
    memcpy(this->memory_end, value, size);

    this->memory_end = new_memory_end;
    return new_value_offset;
}

/// Reallocates the memory with the new size.
void Allocator::resize(size_t size) {
    size_t memory_end_offset = this->memory_end - this->memory_start;

    this->memory_size = size;

    this->memory_start = (char*) realloc(this->memory_start, size);
    this->memory_end = this->memory_start + memory_end_offset;
}

/// Returns the size of currently allocated memory.
offset_t Allocator::size () {
    return this->memory_end - this->memory_start;
}

/// Returns a pointer to the beginning of the allocated memory.
char* Allocator::begin() {
    return this->memory_start;
}

/// Returns a pointer to the end of the allocated memory.
char* Allocator::end() {
    return this->memory_end;
}
}
