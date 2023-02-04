#ifndef INTERPRETER_WORD_H
#define INTERPRETER_WORD_H

#include <shared/definitions.h>
#include <shared/bytecode.h>

#include <interpreter/stacks.h>

namespace ligma {
    
enum wordflag {
    FLAG_EXT = 1,       // value is external
    FLAG_TEMP = 2,      // value is a temporary
    FLAG_LITERAL = 4,   // value is in literal table
    FLAG_READONLY = 8,  // value can't be changed
    FLAG_SYMBOL = 16    // value is in symbol table
};

enum wordtype {
    WORD_ATOM,
    WORD_INT,
    WORD_FLOAT,
    WORD_STRING,
    WORD_BYTECODE,
    WORD_LIST,
    WORD_EXT_INT8,
    WORD_EXT_INT16,
    WORD_EXT_INT32,
    WORD_EXT_INT64,
    WORD_EXT_FLOAT32,
    WORD_EXT_FLOAT64,
    WORD_EXT_STRING,
    WORD_EXT_FUNCTION
};

typedef uint16_t wordflag_t;
typedef wordtype wordtype_t;

struct Word {
    wordflag_t flags;
    wordtype_t type;
    uint16_t length;
    uint16_t refs;
    void (*update_function) (Word*);
    
    struct list {
        param_t data;
        param_t next;
    };
    
    union {
        atom_t atom_value;
        char* string_value;
        int32_t* int_value;
        float* float_value;
        offset_t bytecode_value;
        void* external_value;
        void (*external_function) (RefStack*);
        list list_value;
    };
    
    bool is_external();
    
    bool is_int();
    bool is_float();
    bool is_numeric();
    bool is_string();
    
    int32_t* value_as_int();
    float* value_as_float();
    char* value_as_string();
    
    void insert_int (int_t, offset_t i);
    void insert_float (float_t, offset_t i);
    
    int_t uninsert_int (offset_t i);
    float_t uninsert_float (offset_t i);
    
    int32_t* extract_int();
    float* extract_float();
    
    const char* type_as_string();
    
    void deposit_int(int32_t* value);
    void deposit_float(float* value);
    void deposit_string(char* value);
    
    void make_copy (Word* p);
    
    void make_result_addition (Word* p1, Word* p2);
    void make_result_subtraction (Word* p1, Word* p2);
    void make_result_multiplication (Word* p1, Word* p2);
    void make_result_division (Word* p1, Word* p2);
    void make_result_modulo (Word* p1, Word* p2);
    
    bool is_equal_to (Word* other);
    bool is_not_equal_to (Word* other);
    bool is_greater_than (Word* other);
    bool is_lesser_than (Word* other);
    bool is_greater_or_equal_to (Word* other);
    bool is_lesser_or_equal_to (Word* other);
    
    void clear();
};

}

#endif