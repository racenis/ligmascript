// structures.h
// All rights reserved.
//
// This file contains all of the struct definitions that need to be shared
// across the program, as well as some #define constants, typedefs and also
// small inlineable functions, and also globals variable definitions.

#ifndef STRUCTURES_H
#define STRUCTURES_H

#define INTERPRETER_MAX_HANDLES 1000
#define INTERPRETER_MAX_WORDS 500
#define INTERPRETER_MAX_LITERALS 1000
#define INTERPRETER_MAX_BYTECODE 1000
#define INTERPRETER_MAX_MODULES 100

typedef unsigned short word_handle;
typedef unsigned short word_index;
typedef unsigned short list_index;
typedef unsigned int word_ref;

struct intepreter_module {
    char* name;
    char* filename;
    struct link_in* first_in;
    struct link_in* last_in;
    unsigned char* literals;
    unsigned char* bytecode;
    int literals_last;
    int bytecode_last;
};

struct link_in {
    unsigned short word_name;
    unsigned short code_offset;
    struct link_in* next;
};

// it's used for finding an index of a word from its name
struct interpreter_handle{
    char* name;
    word_index word_offset;
};

// information about a word
struct interpreter_word{
    unsigned char flags;
    unsigned char word_type;
    unsigned short array_length;
    unsigned short refs;
    union{
        struct bytecode{
            unsigned short module;
            unsigned short offset;
        } bytecode;
        void* data_pointer;
        unsigned short next_word;
    };
};


extern struct intepreter_module interpreter_modules[];
extern struct interpreter_handle interpreter_handles[];
extern struct interpreter_word interpreter_words[];
extern unsigned short interpreter_last_word;
extern void (*call_fs[])(void);
extern const unsigned long max_call_fs;


static inline unsigned int hashong (char* const str) {
    unsigned int hash = 2166136261;
    char* c;

    for (c = str; *c != 0; c++){
        //printf("%c", *c);
        hash = hash ^ *c;
        hash = hash * 16777619;
    }

    return hash;
}

#endif // STRUCTURES_H
