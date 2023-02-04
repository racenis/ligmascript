#include <interpreter/stacks.h>
#include <interpreter/interpreter.h>
#include <interpreter/error.h>
#include <stdio.h>
#include <stdlib.h>

namespace ligma {

size_t REFSTACK_SIZE = 25;
size_t JMPSTACK_SIZE = 20;
size_t LCLSTACK_SIZE = 75;
    
RefStack::RefStack () : stack (REFSTACK_SIZE) {}

// TODO: add error checking
void RefStack::push (param_t word_ref) {
    if (stack.is_full()) {
        throw InterpreterError (INTERPRETER_ERROR_REF_STACK_OVERFLOW);
    }
    
    LookupWord (word_ref)->refs++;
    stack.push (word_ref);
}

param_t RefStack::pop () {
    if (stack.is_empty()) {
        throw InterpreterError (INTERPRETER_ERROR_REF_STACK_UNDERFLOW);
    }
    
    param_t word_ref = stack.pop();
    LookupWord (word_ref)->refs--;
    return word_ref;
}

void RefStack::print () {
    // this is terrible
    Stack<param_t> aux_stack (REFSTACK_SIZE);
    while (!stack.is_empty()) {
        aux_stack.push(stack.pop());
    }
    
    while (!aux_stack.is_empty()) {
        param_t ref = aux_stack.pop();
        
        if (ref == 0 ) {
            printf("[NIL] ");
        } else {
            char* str = LookupWord(ref)->value_as_string();
            printf("[%s | %s] ", str, LookupWord(ref)->type_as_string());
            if (LookupWord(ref)->type != WORD_STRING) free (str);
        }
        
        stack.push(ref);
    }
    
    if (stack.is_empty()) printf("[EMPTY]");
    
    printf("\n");
}


JmpStack::JmpStack () : stack (JMPSTACK_SIZE) {}

void JmpStack::push (JmpStackValues values) {
    if (stack.is_full()) {
        throw InterpreterError (INTERPRETER_ERROR_JMP_STACK_OVERFLOW);
    }
    
    stack.push(values);
}

JmpStackValues JmpStack::pop () {
    if (stack.is_empty()) {
        throw InterpreterError (INTERPRETER_ERROR_JMP_STACK_UNDERFLOW);
    }
    
    return stack.pop();
}

bool JmpStack::is_empty () {
    return stack.is_empty();
}


LclStack::LclStack () : stack (LCLSTACK_SIZE)/*, stack_start (LCLSTACK_SIZE)*/ {}

// TODO: actually implement
// TODO: add exceptions
param_t LclStack::push_locals (param_t count) {
    for (param_t i = 0; i < count; i++) {
        param_t w = MakeWord();
        stack.push(w);
        LookupWord(w)->refs++;
    }
    
    //stack_start.push(count);
    
    return 0;
}

param_t LclStack::get_local (param_t local) {
    //param_t c = stack_start.top() - local;
    Stack<param_t> other (10);
    
    for (param_t i = 0; i < local; i++) {
        other.push(stack.pop());
    }
    
    param_t ret = stack.top();
    
    while (!other.is_empty()) {
        stack.push(other.pop());
    }
    
    return ret;
}

void LclStack::pop_locals (param_t count) {
    for (param_t i = 0; i < count; i++) {
        LookupWord(stack.pop())->refs--;
    }
    
    //stack_start.pop();
}

}