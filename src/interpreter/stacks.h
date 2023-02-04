#ifndef INTERPRETER_STACKS_H
#define INTERPRETER_STACKS_H

#include <shared/definitions.h>
#include <shared/stack.h>

namespace ligma {

struct JmpStackValues {
    offset_t offset;
    offset_t file;
    offset_t line;
}; 

class RefStack {
public:
    RefStack ();
    void push (param_t word_ref);
    param_t pop ();
    void print ();
private:
    Stack<param_t> stack;
};

class JmpStack {
public:
    JmpStack ();
    void push (JmpStackValues values);
    JmpStackValues pop (); 
    bool is_empty ();
private:
    Stack<JmpStackValues> stack;
};

class LclStack {
public:
    LclStack ();
    param_t push_locals (param_t count);
    param_t get_local (param_t local);
    void pop_locals (param_t count);
private:
    Stack<param_t> stack;
    //Stack<param_t> stack_start;
    int orange;
};
    
}

#endif

